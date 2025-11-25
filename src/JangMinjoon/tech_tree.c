#include <string.h>
#include <math.h>

#include "tech_tree.h"

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

StatusCodeEnum load_techtrees_from_text(const char *path,
                                    TechTree *trees,
                                    int max_trees,
                                    int *out_count)
{
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
