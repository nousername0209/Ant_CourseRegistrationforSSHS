#include <string.h>
#include <math.h>

#include "tech_tree.h"

#define SUBJECT_FILE   "../dataset/tech_tree/subjects.dat"
#define SCORE_FILE     "../dataset/tech_tree/scores.txt"
#define TREE_FILE      "../dataset/tech_tree/techtrees.txt"
#define STATS_FILE     "../dataset/tech_tree/subject_stats.txt"

static SubjectInfo  g_subjects[MAX_SUBJECTS];
static int          g_subject_count = 0;

static StudentScore g_scores[MAX_SCORES];
static int          g_score_count = 0;

static TechTree     g_trees[MAX_TECH_TREES];
static int          g_tree_count = 0;

/* 표준정규분포를 따르는 확률변수 x를 입력받아 P(z<=x)를 반환한다. */
static double normal_cdf(double x)
{
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

/* 수정 가능한 SubjectInfo 포인터를 찾기 위한 내부 함수 */
static SubjectInfo *find_subject_mutable(SubjectInfo *subjects,
                                         int subject_count,
                                         int subject_id)
{
    for (int i = 0; i < subject_count; i++) {
        if (subjects[i].id == subject_id) {
            return &subjects[i];
        }
    }
    return NULL;
}

const SubjectInfo *find_subject_by_id(const SubjectInfo *subjects,
                                      int subject_count,
                                      int subject_id)
{
    if (subjects == NULL || subject_count < 0) {
        return NULL;
    }

    for (int i = 0; i < subject_count; i++) {
        if (subjects[i].id == subject_id) {
            return &subjects[i];
        }
    }
    return NULL;
}

const SubjectStats *find_subject_stats(const SubjectInfo *subject,
                                       int year,
                                       int semester)
{
    if (subject == NULL) {
        return NULL;
    }

    for (int i = 0; i < subject->stats_count; i++) {
        const SubjectStats *st = &subject->stats[i];
        if (st->year == year && st->semester == semester) {
            return st;
        }
    }
    return NULL;
}

StatusCodeEnum load_subjects_from_text(const char *path,
                                   SubjectInfo *subjects,
                                   int max_subjects,
                                   int *out_count)
{
    if (path == NULL || subjects == NULL || out_count == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (max_subjects <= 0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        *out_count = 0;
        return ERROR_FILE_NOT_FOUND;
    }

    int count = 0;
    while (!feof(fp)) {
        int id;
        char name[NAME_LENGTH];
        int credit;

        /* 한 줄에서 id, name, credit을 읽어 온다. */
        int scanned = fscanf(fp, "%d %63s %d", &id, name, &credit);
        if (scanned == EOF || scanned == 0) {
            break;
        }
        if (scanned != 3) {
            fclose(fp);
            *out_count = 0;
            return STATUS_ERROR_PARSE;
        }
        if (count >= max_subjects) {
            fclose(fp);
            *out_count = 0;
            return ERROR_INDEX_OUT;
        }

        SubjectInfo *s = &subjects[count];
        memset(s, 0, sizeof(SubjectInfo));
        s->id = id;
        strncpy(s->name, name, NAME_LENGTH - 1);
        s->name[NAME_LENGTH - 1] = '\0';
        s->credit = credit;
        s->stats_count = 0;
        s->prereq_count = 0;

        count++;
    }

    fclose(fp);
    *out_count = count;
    return SUCCESS;
}

StatusCodeEnum load_subject_stats_from_text(const char *path,
                                        SubjectInfo *subjects,
                                        int subject_count)
{
    if (path == NULL || subjects == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (subject_count < 0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return ERROR_FILE_NOT_FOUND;
    }

    while (!feof(fp)) {
        int subject_id;
        int year;
        int semester;
        double mean;
        double stdev;

        /* 한 줄에서 subject_id, year, semester, mean, stdev를 읽는다. */
        int scanned = fscanf(fp, "%d %d %d %lf %lf",
                             &subject_id, &year, &semester, &mean, &stdev);
        if (scanned == EOF || scanned == 0) {
            break;
        }
        if (scanned != 5) {
            fclose(fp);
            return STATUS_ERROR_PARSE;
        }

        SubjectInfo *subj = find_subject_mutable(subjects,
                                                 subject_count,
                                                 subject_id);
        if (subj == NULL) {
            fclose(fp);
            return ERROR_FILE_NOT_FOUND;
        }

        if (stdev <= 0.0) {
            fclose(fp);
            return ERROR_INVALID_INPUT;
        }

        /* 같은 (year, semester)가 있으면 덮어쓰고, 없으면 새로 추가 */
        int found_index = -1;
        for (int i = 0; i < subj->stats_count; i++) {
            if (subj->stats[i].year == year &&
                subj->stats[i].semester == semester) {
                found_index = i;
                break;
            }
        }

        SubjectStats *st = NULL;
        if (found_index >= 0) {
            st = &subj->stats[found_index];
        } else {
            if (subj->stats_count >= MAX_SUBJECT_STATS) {
                fclose(fp);
                return ERROR_INDEX_OUT;
            }
            st = &subj->stats[subj->stats_count];
            subj->stats_count++;
        }

        st->year = year;
        st->semester = semester;
        st->mean = mean;
        st->stdev = stdev;
    }

    fclose(fp);
    return SUCCESS;
}

StatusCodeEnum load_techtrees_from_text(const char *path, TechTree *trees,
                                    int max_trees,
                                    int *out_count) {
    if (path == NULL || trees == NULL || out_count == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (max_trees <= 0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        *out_count = 0;
        return ERROR_FILE_NOT_FOUND;
    }

    int tree_index = 0;
    while (!feof(fp)) {
        int tree_id;
        char tree_name[NAME_LENGTH];
        int subject_count;

        /* 첫 줄: 트리 ID, 이름, 포함 과목 수 */
        int scanned = fscanf(fp, "%d %63s %d",
                             &tree_id, tree_name, &subject_count);
        if (scanned == EOF || scanned == 0) {
            break;
        }
        if (scanned != 3) {
            fclose(fp);
            *out_count = 0;
            return STATUS_ERROR_PARSE;
        }

        if (tree_index >= max_trees) {
            fclose(fp);
            *out_count = 0;
            return ERROR_INDEX_OUT;
        }

        if (subject_count < 0 || subject_count > MAX_TREE_SUBJECTS) {
            fclose(fp);
            *out_count = 0;
            return STATUS_ERROR_PARSE;
        }

        TechTree *tree = &trees[tree_index];
        memset(tree, 0, sizeof(TechTree));
        tree->id = tree_id;
        strncpy(tree->name, tree_name, NAME_LENGTH - 1);
        tree->name[NAME_LENGTH - 1] = '\0';
        tree->subject_count = 0;

        /* 다음 subject_count줄: <subject_id> <weight> */
        for (int i = 0; i < subject_count; i++) {
            int subj_id;
            int weight;

            scanned = fscanf(fp, "%d %d", &subj_id, &weight);
            if (scanned != 2) {
                fclose(fp);
                *out_count = 0;
                return STATUS_ERROR_PARSE;
            }

            if (tree->subject_count >= MAX_TREE_SUBJECTS) {
                fclose(fp);
                *out_count = 0;
                return ERROR_INDEX_OUT;
            }

            TreeSubject *node = &tree->subjects[tree->subject_count];
            node->subject_id = subj_id;
            node->weight = weight;
            tree->subject_count++;
        }

        tree_index++;
    }

    fclose(fp);
    *out_count = tree_index;
    return SUCCESS;
}

/**
 * @brief 사용자 성적 데이터를 파일에서 로드한다.
 * * 파일 형식 (각 줄): <subject_id> <year> <semester> <raw_score>
 */
StatusCodeEnum load_student_scores_from_text(StudentScore *scores,
                                             int max_scores,
                                             int *inout_score_count,
                                             int user_id,
                                             const SubjectInfo *subjects,
                                             int subject_count)
{
    char filepath[STR_LENGTH * 2];
    get_score_filename(user_id, filepath);
    
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        // 파일이 없을 경우 (초기 사용자), 오류가 아닌 성공으로 간주하고 count=0 반환
        *inout_score_count = 0;
        return SUCCESS; 
    }

    int subject_id, year, semester;
    double raw_score;
    char line[LINE_LENGTH];
    StatusCodeEnum status = SUCCESS;

    *inout_score_count = 0; // 로드 시작 전 카운트 초기화

    while (fgets(line, sizeof(line), fp)) {
        // 빈 줄 무시
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') continue;

        // <subject_id> <year> <semester> <raw_score> 형식으로 파싱
        if (sscanf(line, "%d %d %d %lf", &subject_id, &year, &semester, &raw_score) == 4) {
            
            // 배열 크기 초과 확인
            if (*inout_score_count >= max_scores) {
                status = ERROR_INDEX_OUT; // 용량 초과 경고
                break; 
            }

            // 기존에 구현된 add_student_score 함수를 사용하여 성적 추가 및 Z-score/Percentile 계산
            StatusCodeEnum add_status = add_student_score(scores, max_scores, inout_score_count, 
                                                            user_id, subject_id, 
                                                            year, semester, 
                                                            raw_score, 
                                                            subjects, subject_count);
            
            // 성적 추가 로직이 실패하면 해당 줄은 무시하고 다음 줄로 진행
            if (add_status != SUCCESS) {
                // 로드 중 데이터 문제 발생 (예: 과목 ID를 찾을 수 없음, 통계 데이터 부족)
                fprintf(stderr, "경고: 성적 로드 중 문제 발생 (User:%d, Subject:%d, Code:%d). 해당 성적 무시.\n", 
                        user_id, subject_id, add_status);
                // inout_score_count는 add_student_score 내에서 증가하지 않았을 것임.
            }
        } else {
            // 파일 형식 오류
            fprintf(stderr, "경고: 성적 파일 형식 오류 발생. 해당 줄 무시: %s", line);
        }
    }

    fclose(fp);
    return status;
}

StatusCodeEnum compute_z_and_percentile(const SubjectInfo *subject,
                                    int year,
                                    int semester,
                                    double raw_score,
                                    double *out_z,
                                    double *out_percentile_top)
{
    if (subject == NULL || out_z == NULL || out_percentile_top == NULL) {
        return ERROR_INVALID_INPUT;
    }

    const SubjectStats *st = find_subject_stats(subject, year, semester);
    if (st == NULL) {
        return ERROR_FILE_NOT_FOUND;
    }
    if (st->stdev <= 0.0) {
        return ERROR_INVALID_INPUT;
    }

    /* (원점수 - 평균) / 표준편차 */
    double z = (raw_score - st->mean) / st->stdev;

    /* 누적분포(아래에 있는 비율) */
    double phi = normal_cdf(z);

    /* 상위 퍼센트 (Top x%) = (1 - phi) * 100 */
    double top_percent = (1.0 - phi) * 100.0;

    *out_z = z;
    *out_percentile_top = top_percent;
    return SUCCESS;
}

StatusCodeEnum add_student_score(StudentScore *scores,
                             int max_scores,
                             int *inout_count,
                             int user_id,
                             int subject_id,
                             int year,
                             int semester,
                             double raw_score,
                             const SubjectInfo *subjects,
                             int subject_count)
{
    if (scores == NULL || inout_count == NULL || subjects == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (max_scores <= 0 || subject_count < 0) {
        return ERROR_INVALID_INPUT;
    }

    if (*inout_count >= max_scores) {
        return ERROR_INDEX_OUT;
    }

    const SubjectInfo *subject =
        find_subject_by_id(subjects, subject_count, subject_id);
    if (subject == NULL) {
        return ERROR_FILE_NOT_FOUND;
    }

    double z;
    double p_top;
    StatusCodeEnum st = compute_z_and_percentile(subject,
                                             year,
                                             semester,
                                             raw_score,
                                             &z,
                                             &p_top);
    if (st != SUCCESS) {
        return st;
    }

    StudentScore *score = &scores[*inout_count];
    score->user_id = user_id;
    score->subject_id = subject_id;
    score->year = year;
    score->semester = semester;
    score->raw_score = raw_score;
    score->z_score = z;
    score->percentile_top = p_top;

    (*inout_count)++;
    return SUCCESS;
}

/* 특정 학생이 특정 과목에서 기록한 성적들 중
 * 가장 좋은(가장 작은) 상위 퍼센트를 찾는다. */
static double find_best_percentile_for_subject(const StudentScore *scores,
                                               int score_count,
                                               int user_id,
                                               int subject_id)
{
    double best = -1.0;

    for (int i = 0; i < score_count; i++) {
        const StudentScore *sc = &scores[i];
        if (sc->user_id != user_id) {
            continue;
        }
        if (sc->subject_id != subject_id) {
            continue;
        }

        if (best < 0.0 || sc->percentile_top < best) {
            best = sc->percentile_top;
        }
    }

    /* 성적이 하나도 없으면 -1.0을 반환하여 "정보 없음"으로 처리 */
    return best;
}

StatusCodeEnum rank_techtrees(const TechTree *trees,
                          int tree_count,
                          const StudentScore *scores,
                          int score_count,
                          int user_id,
                          int *out_indices,
                          double *out_scores,
                          int max_out,
                          int *out_returned)
{
    if (trees == NULL || scores == NULL || out_indices == NULL || out_returned == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (tree_count <= 0 || score_count < 0 || max_out <= 0) {
        return ERROR_INVALID_INPUT;
    }

    /* 임시로 점수와 인덱스를 담을 버퍼 (최대 MAX_TECH_TREES개) */
    double tmp_scores[MAX_TECH_TREES];
    int    tmp_indices[MAX_TECH_TREES];
    int    used_count = 0;

    /* 각 테크트리에 대해 점수 계산 */
    for (int t = 0; t < tree_count; t++) {
        const TechTree *tree = &trees[t];
        double acc = 0.0;
        int used_any = 0;

        for (int i = 0; i < tree->subject_count; i++) {
            int subject_id = tree->subjects[i].subject_id;
            int weight = tree->subjects[i].weight;

            double p_top = find_best_percentile_for_subject(scores,
                                                            score_count,
                                                            user_id,
                                                            subject_id);
            if (p_top < 0.0) {
                /* 이 과목에 대한 성적 기록이 없으면 스킵 */
                continue;
            }

            used_any = 1;
            /* 상위 퍼센트가 낮을수록 좋은 성적이므로 (100 - p_top)을 사용 */
            acc += (100.0 - p_top) * (double)weight;
        }

        if (!used_any) {
            /* 이 테크트리에서 활용 가능한 성적이 하나도 없으면 제외 */
            continue;
        }

        /* 임시 버퍼에 저장 */
        if (used_count < MAX_TECH_TREES) {
            tmp_scores[used_count] = acc;
            tmp_indices[used_count] = t;
            used_count++;
        } else {
            /* 이론상 tree_count <= MAX_TECH_TREES라면 안 들어오지만, 방어 코드 */
            break;
        }
    }

    if (used_count == 0) {
        *out_returned = 0;
        return ERROR_FILE_NOT_FOUND;
    }

    /* 점수 기준 내림차순 정렬 (간단한 선택 정렬) */
    for (int i = 0; i < used_count - 1; i++) {
        int best_idx = i;
        for (int j = i + 1; j < used_count; j++) {
            if (tmp_scores[j] > tmp_scores[best_idx]) {
                best_idx = j;
            }
        }
        if (best_idx != i) {
            double ts = tmp_scores[i];
            tmp_scores[i] = tmp_scores[best_idx];
            tmp_scores[best_idx] = ts;

            int ti = tmp_indices[i];
            tmp_indices[i] = tmp_indices[best_idx];
            tmp_indices[best_idx] = ti;
        }
    }

    /* 요청한 개수(max_out)와 실제 개수 중 작은 값만큼만 반환 */
    int out_n = (used_count < max_out) ? used_count : max_out;
    for (int i = 0; i < out_n; i++) {
        out_indices[i] = tmp_indices[i];
        if (out_scores != NULL) {
            out_scores[i] = tmp_scores[i];
        }
    }

    *out_returned = out_n;
    return SUCCESS;
}

static StatusCodeEnum popup_enter_grade(Subject **subjects_to_enter,
                                        int count,
                                        User *user,
                                        SubjectInfo *subjects,
                                        int subject_count,
                                        StudentScore *scores,
                                        int max_scores,
                                        int *inout_score_count) {
    if (count <= 0) return SUCCESS;

    int selected_idx = 0;
    int box_w = 40;
    int box_h = count + 6;
    int start_x = (CONSOLE_WIDTH - box_w) / 2;
    int start_y = 5;
    int ch;

    // 배경 그리기
    for(int i=0; i<box_h; i++) {
        goto_ansi(start_x, start_y + i);
        printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
    }

    while(1) {
        // 제목
        goto_ansi(start_x + 2, start_y + 1);
        printf("%s 성적 입력 과목 선택 %s", UI_BOLD, UI_RESET); 

        // 과목 목록
        for(int i=0; i<count; i++) {
            Subject *s = subjects_to_enter[i];
            goto_ansi(start_x + 2, start_y + 3 + i);
            
            char display_text[STR_LENGTH];
            sprintf(display_text, "%s (%d학점, %d학기)", s->name, s->credit, s->semester);

            if (i == selected_idx) {
                printf("%s%s > %s %s", UI_REVERSE, UI_COLOR_YELLOW, display_text, UI_RESET);
            } else {
                printf("%s   %s %s", UI_REVERSE, display_text, UI_RESET);
            }
        }
        
        // 안내
        goto_ansi(start_x + 2, start_y + box_h - 2);
        printf("선택: ENTER, 취소: ESC");

        goto_ansi(1, 1);
        ch = getch(); // struct.h의 getch() 사용

        if (ch == 224 || ch == 0) {
            ch = getch();
            if (ch == UP_ARROW && selected_idx > 0) selected_idx--;
            if (ch == DOWN_ARROW && selected_idx < count - 1) selected_idx++;
        }
        else if (ch == ENTER) {
            Subject *selected_subj = subjects_to_enter[selected_idx];
            
            // 팝업 지우기
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }

            double raw_score = 0.0;
            
            // 성적 입력 프롬프트
            goto_ansi(start_x, start_y);
            printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET);
            goto_ansi(start_x + 2, start_y + 1);
            printf("%s %s 점수 입력 (0.0~100.0): %s", UI_BOLD, selected_subj->name, UI_RESET); 
            goto_ansi(start_x + 2, start_y + 2);
            printf("> Raw Score: ");
            
            // TUI 환경에서 입력 단순화
            scanf("%lf", &raw_score); 
            
            // 성적 추가 또는 수정
            int year = 2024; // 임시 연도 설정
            StatusCodeEnum status;
            
            // 기존 성적 확인
            int found_index = -1;
            for (int i = 0; i < *inout_score_count; i++) {
                if (scores[i].user_id == user->id && scores[i].subject_id == selected_subj->id) {
                    found_index = i; // 같은 과목 ID가 있는 경우 찾음
                    break;
                }
            }
            
            if (found_index != -1) {
                // 이미 성적이 있으면 수정
                scores[found_index].raw_score = raw_score;
                const SubjectInfo *info = find_subject_by_id(subjects, subject_count, selected_subj->id);
                if (info != NULL) {
                    status = compute_z_and_percentile(info, year, selected_subj->semester, 
                                                      raw_score, 
                                                      &scores[found_index].z_score, 
                                                      &scores[found_index].percentile_top);
                } else {
                    status = ERROR_FILE_NOT_FOUND;
                }
            } else {
                // 새 성적 추가
                status = add_student_score(scores, max_scores, inout_score_count, 
                                           user->id, selected_subj->id, 
                                           year, selected_subj->semester, 
                                           raw_score, 
                                           subjects, subject_count);
            }
            
            // 결과 메시지 표시
            goto_ansi(start_x, start_y);
            printf("%*s", box_w, "");
            goto_ansi(start_x + 2, start_y + 1);
            printf("%*s", box_w - 4, "");
            goto_ansi(start_x + 2, start_y + 2);
            printf("%*s", box_w - 4, "");
            
            goto_ansi(start_x + 2, start_y + 1);
            if (status == SUCCESS) {
                printf("%s%s 성적 입력 완료. %s", UI_COLOR_GREEN, selected_subj->name, UI_RESET);
            } else {
                printf("%s성적 입력 오류 (Code: %d)%s", UI_COLOR_YELLOW, status, UI_RESET);
            }
            getch(); // 확인 대기
            
            // 다시 목록 팝업 그리기
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%s%*s%s", UI_REVERSE, box_w, "", UI_RESET); 
            }
        }
        else if (ch == ESC) {
            // 팝업 지우기
            for(int i=0; i<box_h; i++) {
                goto_ansi(start_x, start_y + i);
                printf("%*s", box_w, ""); 
            }
            return SUCCESS;
        }
    }
}


/* ==========================================================
 * Tech Tree 메인 UI 함수
 * ========================================================== */

StatusCodeEnum run_grade_input_ui(User *user,
                                SubjectInfo *subjects,
                                int subject_count,
                                StudentScore *scores,
                                int max_scores,
                                int *inout_score_count) {
    // 이수한 과목 목록을 수집 (user->current_sem까지의 학기)
    Subject *taken_subjects[SEMESTER_NUM * MAX_SUBJECT_NUM];
    int count = 0;

    for (int sem_idx = 0; sem_idx < user->current_sem; sem_idx++) {
        TimeTable *t = user->table[sem_idx];
        if (t != NULL) {
            for (int i = 0; i < t->n; i++) {
                if (count < SEMESTER_NUM * MAX_SUBJECT_NUM) {
                    taken_subjects[count++] = t->subjects[i];
                }
            }
        }
    }

    if (count == 0) {
        goto_ansi(START_X, START_Y + 10);
        printf("%s%s이수한 과목이 없습니다.%s", UI_COLOR_YELLOW, UI_BOLD, UI_RESET);
        getch();
        return SUCCESS;
    }
    
    // 성적 입력 팝업 호출
    return popup_enter_grade(taken_subjects, count, 
                             user, subjects, subject_count,
                             scores, max_scores, inout_score_count);
}

/**
 * @brief 계산된 테크트리 점수 순위를 화면에 표시하는 UI를 제공한다.
 */
StatusCodeEnum run_recommendation_ui(const TechTree *trees,
                                   int tree_count,
                                   const StudentScore *scores,
                                   int score_count,
                                   const SubjectInfo *subjects,
                                   int subject_count,
                                   int user_id) {
    int max_out = MAX_TECH_TREES;
    int recommended_indices[MAX_TECH_TREES];
    double recommended_scores[MAX_TECH_TREES];
    int returned_count;

    // 1. 테크트리 점수 계산 및 순위 정렬 (제공된 내부 함수 사용)
    StatusCodeEnum status = compute_tech_tree_scores(
        trees, tree_count, 
        scores, score_count, 
        user_id, 
        recommended_indices, 
        recommended_scores, 
        max_out, 
        &returned_count);

    // 화면 초기화 및 헤더 출력
    system("cls || clear");
    goto_ansi(1, 1);
    printf("%s================================================================================%s", UI_BOLD, UI_RESET);
    goto_ansi(1, 2);
    printf("%s   [Tech Tree 추천 결과]   %s", UI_BOLD, UI_RESET);
    goto_ansi(1, 3);
    printf("%s================================================================================%s", UI_BOLD, UI_RESET);

    if (status != SUCCESS || returned_count == 0) {
        goto_ansi(10, 5);
        if (status == ERROR_FILE_NOT_FOUND || returned_count == 0) {
            printf("%sERROR: 활용 가능한 성적 데이터가 없거나, 통계 데이터가 부족하여 추천할 수 없습니다.%s", UI_COLOR_YELLOW, UI_RESET);
        } else {
            printf("%sERROR: 추천 점수 계산 중 오류 발생 (Code: %d)%s", UI_COLOR_YELLOW, status, UI_RESET);
        }
        goto_ansi(1, 10);
        printf("아무 키나 눌러 돌아가기...");
        getch();
        return status;
    }

    // 2. 순위 목록 표시
    goto_ansi(3, 5);
    printf("%s순위  테크트리 이름                  점수%s", UI_BOLD, UI_RESET);
    goto_ansi(3, 6);
    printf("------------------------------------------------------------------");

    for (int i = 0; i < returned_count; i++) {
        const TechTree *tree = &trees[recommended_indices[i]];
        double score = recommended_scores[i];
        
        goto_ansi(3, 7 + i);
        // 1위는 녹색으로 강조
        printf("%s%2d%s. ", (i==0) ? UI_COLOR_GREEN : UI_RESET, i + 1, UI_RESET);
        printf("%-30s ", tree->name);
        printf("%s%.2f%s", (i==0) ? UI_COLOR_GREEN : UI_RESET, score, UI_RESET);
    }
    
    // 3. 1위 테크트리 상세 정보 표시
    int detail_y = 10 + returned_count;
    const TechTree *top_tree = &trees[recommended_indices[0]];

    goto_ansi(1, detail_y);
    printf("%s================================================================================%s", UI_BOLD, UI_RESET);
    goto_ansi(3, detail_y + 1);
    printf("%s[TOP 1 추천 테크트리: %s]%s", UI_BOLD, top_tree->name, UI_RESET);
    goto_ansi(3, detail_y + 3);
    printf("%s점수 기여 과목 목록:%s", UI_BOLD, UI_RESET);

    int current_y = detail_y + 4;
    
    for (int i = 0; i < top_tree->subject_count; i++) {
        const TreeSubject *ts = &top_tree->subjects[i];
        // 과목 정보 검색 (제공된 내부 함수 사용)
        const SubjectInfo *subj_info = find_subject_by_id(subjects, subject_count, ts->subject_id);

        if (subj_info != NULL) {
            // 이 과목의 가장 좋은 성적(최소 percentile_top) 찾기
            double best_percentile = 101.0;
            int found_score = 0;
            for (int j = 0; j < score_count; j++) {
                if (scores[j].user_id == user_id && scores[j].subject_id == ts->subject_id) {
                    if (scores[j].percentile_top < best_percentile) {
                        best_percentile = scores[j].percentile_top;
                    }
                    found_score = 1;
                }
            }
            
            goto_ansi(5, current_y++);
            printf("- %s (%d학점, 가중치: %d)", subj_info->name, subj_info->credit, ts->weight);
            
            if (found_score) {
                // 점수 계산 공식: (100 - percentile_top) * weight 
                double contribution = (100.0 - best_percentile) * (double)ts->weight;
                goto_ansi(40, current_y - 1);
                printf("| 기여 점수: %s%.2f%s (상위 %.2f%%)", UI_COLOR_CYAN, contribution, UI_RESET, best_percentile);
            } else {
                goto_ansi(40, current_y - 1);
                printf("| %s성적 없음 (점수 계산에 미반영)%s", UI_DIM, UI_RESET);
            }
        }
    }

    goto_ansi(1, CONSOLE_WIDTH - 2);
    printf("아무 키나 눌러 돌아가기...");
    getch();

    system("cls || clear");

    return SUCCESS;
}

/**
 * @brief 사용자 ID를 받아 테크트리 시스템의 전체 로직 (성적 입력 및 추천 UI)을 실행한다.
 */
StatusCodeEnum run_tech_tree_system(int user_id) {
    User user = {0};
    int is_first = 0; // 사용자 데이터 파일이 없는 경우
    
    SubjectInfo *subjects = NULL;
    int subject_count = 0;
    
    TechTree *trees = NULL;
    int tree_count = 0;
    
    // 학생 성적은 동적 배열로 관리
    StudentScore *scores = NULL;
    int score_count = 0;
    int max_scores = MAX_SCORES;
    
    StatusCodeEnum status = SUCCESS;

    // 1. 사용자 데이터 로드 (struct.h의 함수 사용)
    // NOTE: load_user_data는 과목 트리가 로드된 후 실행되어야 하지만,
    // 이 예시에서는 User struct에 timetable만 로드한다고 가정하고 진행.
    // TUI 메인에서 과목 트리를 로드하는 것이 일반적이지만, 여기서는 생략하고
    // user.table에 이수한 과목 목록이 로드되었다고 가정함.
    load_user_data(&user, user_id, &is_first);
    user.id = user_id;
    
    // 2. 과목 정보 및 통계 로드
    status = load_subjects_from_text(SUBJECT_FILE, subjects, MAX_SUBJECT_NUM, &subject_count);
    status = load_subject_stats_from_text(STATS_FILE, subjects, subject_count);
    if (status != SUCCESS) {
        printf("ERROR: 과목 데이터 로드 실패 (Code: %d)\n", status);
        goto cleanup;
    }
    
    // 3. 테크트리 정보 로드 (tech_tree.c의 함수 사용)
    // load_techtrees_from_text 함수가 TechTree 배열과 그 크기를 반환하도록 가정
    status = load_techtrees_from_text(TREE_FILE, trees, MAX_TECH_TREES, &tree_count);
    if (status != SUCCESS) {
        printf("ERROR: 테크트리 로드 실패 (Code: %d)\n", status);
        goto cleanup;
    }

    // 4. 기존 성적 로드 (선택 사항: 파일에서 기존 성적 로드하는 함수가 있다고 가정)
    // status = load_student_scores_from_text(&scores, &score_count, &max_scores, user_id);
    // 이 예시에서는 성적 파일 로드 함수는 구현하지 않고, 초기 성적 배열만 생성.
    scores = (StudentScore*)calloc(MAX_SCORES, sizeof(StudentScore));
    if (scores == NULL) {
        status = ERROR_MEMORY_ALLOC;
        goto cleanup;
    }

    // 5. 성적 입력 UI 실행
    // draw_screen 등을 호출하여 메인 화면을 먼저 그린 후 팝업을 띄우는 것이 좋으나,
    // 여기서는 간단히 팝업만 띄우는 run_grade_input_ui를 호출합니다.
    system("cls || clear");
    goto_ansi(1, 1);
    printf("%s[1. 성적 입력 단계]%s\n", UI_BOLD, UI_RESET);
    run_grade_input_ui(&user, subjects, subject_count, scores, max_scores, &score_count);

    // 6. 테크트리 추천 UI 실행
    system("cls || clear");
    goto_ansi(1, 1);
    printf("%s[2. 테크트리 추천 단계]%s\n", UI_BOLD, UI_RESET);
    run_recommendation_ui(trees, tree_count, (const StudentScore*)scores, score_count, 
                          (const SubjectInfo*)subjects, subject_count, user_id);

    // 7. 성적 데이터 저장 (선택 사항: 성적을 파일에 저장하는 함수가 있다고 가정)
    // save_student_scores_to_text(scores, score_count, user_id);

cleanup:
    // 메모리 해제
    if (subjects) free(subjects);
    if (trees) free(trees);
    if (scores) free(scores);
    
    // User 구조체의 timetable 내부 Subject 노드들 해제 (필요시)
    // free_user_timetable(&user); 

    // 최종 상태 반환
    return status;
}