#include <string.h>
#include <math.h>

#include "tech_tree.h"

#define SUBJECT_FILE   "./dataset/tech_tree/subjects.txt"
#define SCORE_FILE     "./dataset/tech_tree/scores.txt"
#define TREE_FILE      "./dataset/tech_tree/techtrees.txt"
#define STATS_FILE     "./dataset/tech_tree/subject_stats.txt"

static SubjectInfo  g_subjects[MAX_SUBJECT_NUM];
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

const SubjectInfo *find_subject_by_id(SubjectInfo *subjects,
                                      int subject_count,
                                      int subject_id) {
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
                                       int semester) {
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
                                   int *out_count) {
    if (path == NULL || subjects == NULL || out_count == NULL) {
        if (path == NULL) return ERROR_INVALID_INPUT;
        else if (subjects == NULL) return ERROR_DEBUG;
        else return ERROR_DEBUG;
    }

    if (max_subjects <= 0) {
        return ERROR_DEBUG;
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
        int scanned = fscanf(fp, "%d|%[^|]|%d", &id, name, &credit);
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
                                        int subject_count) {
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

        SubjectInfo *subj = find_subject_by_id(subjects,
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

StatusCodeEnum compute_z_and_percentile(const SubjectInfo *subject,
                                    int year,
                                    int semester,
                                    double raw_score,
                                    double *out_z,
                                    double *out_percentile_top) {
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
                             int subject_count) {
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

static double find_best_percentile_for_subject(const StudentScore *scores,
                                               int score_count,
                                               int user_id,
                                               int subject_id) {    
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
                          int *out_returned) {
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

StatusCodeEnum save_scores(const StudentScore *scores, int score_count) {
    FILE * fp = fopen(SCORE_FILE, "wb");
    fwrite(&score_count, sizeof(int), 1, fp);
    fwrite(scores, sizeof(StudentScore), score_count, fp);
    fclose(fp);
}

StatusCodeEnum load_scores(StudentScore *scores, int max_scores, int *out_count) {
    FILE * fp = fopen(SCORE_FILE, "rb");
    if (fp == NULL) {
        *out_count = 0;
        return ERROR_FILE_NOT_FOUND;
    }
    int score_count = 0;
    fread(&score_count, sizeof(int), 1, fp);
    if (score_count > max_scores) {
        fclose(fp);
        *out_count = 0;
        return ERROR_INDEX_OUT;
    }
    fread(scores, sizeof(StudentScore), score_count, fp);
    fclose(fp);
    *out_count = score_count;
    return SUCCESS;
}


/**
 * @brief 주어진 시간표(TimeTable)의 과목들에 대해 원점수(raw_score)를 입력받는 팝업 UI를 그린다.
 * * 이 함수는 output.c의 popup_input_difficulty 함수와 유사하게 작동하도록 구현.
 * * @param t               성적을 입력할 과목 목록 (TimeTable)
 * @param raw_scores      입력된 원점수(0~100)를 저장할 배열
 * @param current_idx     현재 선택된 입력 필드 인덱스 (0~n-1: 과목 인덱스)
 * @param box_w, box_h, start_x, start_y 팝업 박스 위치/크기
 * * @return void
 */
void draw_score_input_popup(TimeTable* t, double raw_scores[], int current_idx, int box_w, int box_h, int start_x, int start_y) {
    if (t == NULL) return;

    // 과목 목록과 입력 필드 표시
    for (int i = 0; i < t->n; i++) {
        Subject* s = t->subjects[i];
        int y_pos = start_y + 3 + i * 2;
        
        // 과목 이름
        goto_ansi(start_x + 2, y_pos);
        printf("%-25s", s->name);

        // 원점수 입력 필드
        goto_ansi(start_x + 30, y_pos);
        if (i == current_idx) {
            printf(UI_REVERSE "  %.1f  " UI_RESET, raw_scores[i]);
        } else {
            printf("  %.1f  ", raw_scores[i]);
        }
    }
    
    // 버튼 그리기
    int btn_y = start_y + box_h - 3;
    goto_ansi(start_x + box_w / 2 - 4, btn_y);
    printf(UI_COLOR_GREEN " [ 저장 ] " UI_RESET);
}

/**
 * @brief 난이도 입력 값이 유효한지 검사한다 (0~100 사이 실수)
 * * @param buf 입력 문자열
 * * @return int 유효하면 1, 아니면 0 반환
 */
int is_valid_score_input(const char* buf) {
    int val;
    // 입력 버퍼가 비어있지 않고, 숫자로 시작하며, 소수점 이하 첫째 자리까지만 유효하도록 간소화
    if (sscanf(buf, "%d", &val) != 1) {
        return 0; // 숫자로 파싱 실패
    }
    // 0.0 이상 100.0 이하인지 확인
    return (val >= 0 && val <= 1000);
}

/**
 * @brief 성적 입력 팝업 UI를 실행하고 입력된 데이터를 반환한다.
 * * @param t               성적을 입력할 과목 목록 (TimeTable)
 * @param raw_scores      입력된 원점수(0~100)를 저장할 배열
 * * @return StatusCodeEnum 성공 시 SUCCESS, 취소 시 ERROR_CANCEL
 */
StatusCodeEnum popup_input_scores(TimeTable* t, double raw_scores[], int user_id, int year, int semester) {
    if (t == NULL || t->n == 0) return SUCCESS;

    const int box_w = 40;
    const int box_h = 6 + t->n * 2;
    const int start_x = (80 - box_w) / 2;
    const int start_y = (30 - box_h) / 2;
    
    int current_idx = 0; // 0 ~ t->n - 1
    char input_buf[STR_LENGTH] = "";

    // 초기 성적 값은 모두 0.0으로 설정
    for (int i = 0; i < t->n; i++) {
        raw_scores[i] = 0.0;
    }

    while (1) {
        system("cls");
        draw_score_input_popup(t, raw_scores, current_idx, box_w, box_h, start_x, start_y);

        // 현재 선택된 입력 필드의 위치
        int x_pos = start_x + 32;
        int y_pos = start_y + 3 + current_idx * 2;
        
        // 입력 필드에 커서 위치시키기
        goto_ansi(x_pos, y_pos);

        // 입력 처리
        int ch = getch();

        if (ch == ENTER) {
            // 저장 버튼을 누른 경우 (마지막 과목에서 ENTER)
            if (current_idx == t->n) {
                add_student_score(g_scores, MAX_SCORES, &g_score_count,
                                  user_id, // user_id는 나중에 채워짐
                                  t->subjects[0]->id, // subject_id는 나중에 채워짐
                                  year, // year는 나중에 채워짐
                                  semester, // semester는 나중에 채워짐
                                  raw_scores[0], // raw_score는 나중에 채워짐
                                  g_subjects,
                                  g_subject_count);
                return SUCCESS;
            }
            // 다음 필드로 이동
            current_idx++;
            if (current_idx > t->n) current_idx = t->n; // t->n은 저장 버튼 인덱스
        } 
        else if (ch == ESC) {
            return ERROR_CANCEL; // 취소
        } 
        else if (ch == UP_ARROW) {
            if (current_idx > 0) current_idx--;
        } 
        else if (ch == DOWN_ARROW) {
            if (current_idx < t->n) current_idx++;
        } 
        else {
            // 성적 입력 처리
            int max_len = 5; // 예: 100.0
            
            // 현재 입력 필드의 값으로 input_buf 초기화
            int current_score_times10 = (int)(raw_scores[current_idx]*10+0.1);
            sprintf(input_buf, "%d", current_score_times10);
            
            if (ch == BACKSPACE) {
                if (strlen(input_buf) > 1) {
                    input_buf[strlen(input_buf) - 1] = '\0';
                } else if (strlen(input_buf) == 1) {
                    input_buf[0] = '0'; input_buf[1] = '\0';
                }
            } else if (ch >= '0' && ch <= '9') {
                if (strlen(input_buf) < max_len) {
                    char temp[2] = {(char)ch, '\0'};
                    strcat(input_buf, temp);
                }
            }

            // 유효성 검사 및 값 업데이트
            if (is_valid_score_input(input_buf)) {
                current_score_times10 = atoi(input_buf);
                raw_scores[current_idx] = (float)current_score_times10 /10.0f;
            }
        }

        if (current_idx == t->n) {
            // 마지막 인덱스는 "저장" 버튼
            goto_ansi(start_x + box_w / 2 - 4, start_y + box_h - 3);
            printf(UI_REVERSE UI_COLOR_GREEN " [ 저장 ] " UI_RESET);
        }
    }
}

/**
 * @brief 성적 입력 및 저장 시스템을 실행한다.
 * * user.current_sem - 1 학기까지의 이수 과목에 대해
 * raw_score(원점수)를 입력받아 StudentScore 구조체에 저장하고 파일에 추가한다.
 * * @param user          사용자 정보 구조체 (이수 과목 정보 포함)
 * @param scores        성적 배열의 포인터
 * @param score_count   성적 개수의 포인터
 * @param max_scores    성적 배열의 최대 크기
 * @param student_id    대상 학생 ID
 * * @return StatusCodeEnum 성공 시 SUCCESS, 실패 시 오류 코드
 */
StatusCodeEnum run_score_input_system(User* user, 
                                        StudentScore *scores, 
                                        int *score_count, 
                                        int max_scores, 
                                        int student_id) 
{
    // 마지막 학기를 제외한 기이수 학기 수 (1학기부터 user->current_sem - 1 학기까지)
    int end_sem_idx = user->current_sem - 1; 

    // 모든 학기의 과목을 담을 임시 리스트
    TimeTable temp_table = {0};

    // 1. 성적 입력 대상 과목 수집 (1학기부터 end_sem_idx 학기까지)
    for (int sem_idx = 0; sem_idx < end_sem_idx && sem_idx < SEMESTER_NUM; sem_idx++) {
        TimeTable* t = user->table[sem_idx];
        if (t != NULL) {
            for (int i = 0; i < t->n; i++) {
                Subject* s = t->subjects[i];
                if (s != NULL && temp_table.n < MAX_SUBJECT_NUM) {
                    temp_table.subjects[temp_table.n++] = s;
                }
            }
        }
    }

    if (temp_table.n == 0) {
        popup_show_message("정보", "현재 학기 이전까지 이수한 과목이 없습니다.");
        return SUCCESS;
    }

    // 2. 성적 입력
    double raw_scores[MAX_SUBJECT_NUM] = {0.0};
    StatusCodeEnum status = popup_input_scores(&temp_table, raw_scores, year, semester, student_id);

    if (status == ERROR_CANCEL) {
        return ERROR_CANCEL;
    }

    // 3. 입력된 성적을 StudentScore 배열에 추가
    for (int i = 0; i < temp_table.n; i++) {
        Subject* s = temp_table.subjects[i];
        double score = raw_scores[i];
        
        // 기존 성적 데이터 중복 체크 및 업데이트 (만약 이미 성적이 있다면)
        int found = 0;
        for (int j = 0; j < *score_count; j++) {
            if (scores[j].user_id == student_id && scores[j].subject_id == s->id) {
                // 이미 성적 데이터가 있으면 업데이트 (가장 최근 입력된 점수로)
                scores[j].raw_score = score;
                found = 1;
                break;
            }
        }

        // 새로운 성적 데이터 추가
        if (!found) {
            if (*score_count < max_scores) {
                scores[*score_count].user_id = student_id;
                scores[*score_count].subject_id = s->id;
                scores[*score_count].year = 0; // 성적 입력 시스템에서는 년도/학기 정보는 사용 안함
                scores[*score_count].semester = 0; 
                scores[*score_count].raw_score = score;
                (*score_count)++;
            } else {
                popup_show_message("오류", "성적 배열이 가득 찼습니다. 일부 데이터는 저장되지 않습니다.");
                break;
            }
        }
    }
    
    // 4. StudentScore 데이터를 파일에 저장
    status = save_scores(scores, *score_count);
    if (status != SUCCESS) {
        popup_show_message("오류", "성적 데이터 파일 저장 실패.");
        return status;
    }

    popup_show_message("성공", "성적 입력이 완료되고 데이터가 저장되었습니다.");
    return SUCCESS;
}

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
    StatusCodeEnum status = rank_techtrees(
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
 * @brief 테크트리 시스템의 메인 함수. 성적 입력, 계산, 출력을 수행한다.
 * * @param student_id 대상 학생 ID
 */
void run_tech_tree(int student_id) {
    User user = {0};
    int is_first = 0;

    // 1. 사용자 이수 과목 데이터 로드 (input.c의 함수 사용)
    load_user_data(&user, student_id, &is_first);
    if (user.id == 0) {
        // load_user_data가 실패했거나 사용자 데이터가 없음
        popup_show_message("오류", "사용자 과목 데이터 로드 실패.");
        return;
    }

    // 2. 과목/통계/테크트리 정보 로드
    StatusCodeEnum status = load_subjects_from_text(SUBJECT_FILE, g_subjects, MAX_SUBJECT_NUM, &g_subject_count);
    if (status != SUCCESS) {
        goto_ansi(0, 0);
        printf("ERROR: 과목 데이터 로드 실패 (Code: %d)\n", status);
        popup_show_message("오류", "테크트리 시스템 초기화 실패: 과목 데이터 로드 실패.");
        return;
    }
    
    status = load_subject_stats_from_text(STATS_FILE, g_subjects, g_subject_count);
    if (status != SUCCESS) {
        printf("ERROR: 과목 통계 로드 실패 (Code: %d)\n", status);
        // 통계 데이터 없어도 시스템 진행 가능하도록 오류 메시지만 표시
        popup_show_message("경고", "과목 통계 데이터 로드 실패. 일부 계산이 정확하지 않을 수 있습니다.");
    }
    
    status = load_techtrees_from_text(TREE_FILE, g_trees, MAX_TECH_TREES, &g_tree_count);
    if (status != SUCCESS) {
        printf("ERROR: 테크트리 로드 실패 (Code: %d)\n", status);
        popup_show_message("오류", "테크트리 시스템 초기화 실패: 테크트리 정보 로드 실패.");
        return;
    }

    // 3. 기존 성적 로드
    // g_scores, g_score_count 전역 변수 사용
    StudentScore *scores = g_scores;
    int *score_count = &g_score_count;
    int max_scores = MAX_SCORES;
    
    load_scores(scores, max_scores, score_count);
    
    // 4. 성적 입력 UI 실행
    status = run_score_input_system(&user, scores, score_count, max_scores, student_id);

    // 사용자가 취소했으면 테크트리 계산은 생략
    if (status == ERROR_CANCEL) {
        popup_show_message("취소", "성적 입력이 취소되었습니다.");
        return;
    }
    
    // 5. 테크트리 순위 계산
    int out_indices[MAX_TECH_TREES];
    double out_scores[MAX_TECH_TREES];
    int num_results = 0;

    rank_techtrees(g_trees, g_tree_count, scores, *score_count, student_id, 
                            out_indices, out_scores, MAX_TECH_TREES, &num_results);

    // 6. 결과 출력
    run_recommendation_ui(g_trees, g_tree_count, g_scores, g_score_count, g_subjects, g_subject_count, student_id);

    // 임시로 로드한 user 데이터 해제 (free_user는 struct.h에 정의되어 있다고 가정)
    // free_user(&user);
}