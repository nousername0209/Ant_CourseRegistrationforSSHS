//
// Created by 김주환 on 25. 11. 9.
//
// Modified by 장민준 on 2025.11.24. (파일 저장/로드 함수 추가)

#include "../struct.h"
#include "recommender.h"
#include <string.h>

/**
 * @brief SubjectInfo 안에서 특정 year/semester에 해당하는 원점수 평균/표준편차 통계(SubjectStats)를 찾는다.
 *
 * @param subject_info  통계를 찾을 과목 SubjectInfo 구조체 포인터
 * @param year          찾고자 하는 연도
 * @param semester      찾고자 하는 학기 (1, 2, 3)
 * @return const SubjectStats*
 *         - 찾으면 해당 SubjectStats의 포인터
 *         - 없으면 NULL
 */

const SubjectStats *find_subject_stats(const SubjectInfo *subject_info, int year, int semester)
{
    if (subject_info == NULL) return NULL;

    for (int i = 0; i < subject_info->stats_count; i++) {
        const SubjectStats *st = &subject_info->stats[i];

        if (st->year == year && st->semester == semester) {
            return st;
        }
    }
    return NULL;
}

/**
 * @brief SubjectInfo에 새로운 통계(SubjectStats)를 하나 추가한다.
 *
 * @param subject_info    통계를 추가할 과목 SubjectInfo 구조체 포인터
 * @param year            연도
 * @param semester        학기
 * @param mean_raw_score  해당 연도/학기의 원점수 평균
 * @param stdev_raw_score 해당 연도/학기의 원점수 표준편차
 * @return StatusCode
 *         - SUCCESS: 추가 성공
 *         - ERROR_INVALID_INPUT: subject_info가 NULL일 때
 *         - ERROR_INDEX_OUT: MAX_SUBJECT_STATS를 초과했을 때 등
 */
StatusCode add_subject_stats(SubjectInfo *subject_info, int year, int semester, double mean_raw_score, double stdev_raw_score)
{
    if (subject_info == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (subject_info->stats_count >= MAX_SUBJECT_STATS) {
        return ERROR_INDEX_OUT;
    }

    SubjectStats *st = &subject_info->stats[subject_info->stats_count];
    st->year = year;
    st->semester = semester;
    st->mean_raw_score = mean_raw_score;
    st->stdev_raw_score = stdev_raw_score;

    subject_info->stats_count++;
    return SUCCESS;
}

/**
 * @brief 학생이 특정 과목의 선수과목들을 모두 들었는지 검사한다.
 *
 * @param subject_info  선수과목 정보를 가진 과목 SubjectInfo 구조체 포인터
 * @param taken_ids     학생이 이미 수강한 과목 id 배열
 * @param taken_count   taken_ids 배열에 들어있는 과목 개수
 * @return int
 *         - 1: 모든 선수과목 id가 taken_ids 안에 있음 (선수요건 만족)
 *         - 0: 하나라도 빠져 있음 (선수요건 불만족)
 */
int meets_prereq(const SubjectInfo *subject_info, const int taken_ids[], int taken_count)
{
    if (subject_info == NULL || taken_ids == NULL) {
        return 0;
    }

    for (int i = 0; i < subject_info->prereq_count; i++) {
        int need_id = subject_info->prereq_ids[i];
        int found = 0;

        for (int j = 0; j < taken_count; j++) {
            if (taken_ids[j] == need_id) {
                found = 1;
                break;
            }
        }

        if (!found) {
            return 0;   // 하나라도 없으면 선수요건 불만족
        }
    }

    return 1;           // 전부 포함
}

/* ============================================================
 *  Z-score 관련 함수
 * ============================================================ */

/**
 * @brief 특정 연도/학기에서의 성적을 이용해 z-score를 계산하고, SubjectZScore 배열에 저장한다.
 *
 * @param subject_info   z-score를 계산할 과목 정보
 * @param year           기준 연도
 * @param semester       기준 학기
 * @param raw_score      학생의 원점수
 * @param z_array        (과목, z_score) 정보를 저장하는 배열
 * @param z_index        z_array에 쓸 인덱스 (0 <= z_index < 배열 크기)
 * @return StatusCode
 *         - SUCCESS: 정상 계산
 *         - ERROR_INVALID_INPUT: subject_info나 z_array가 NULL, 통계를 찾지 못했을 때 등
 *         - ERROR_INDEX_OUT: z_index가 배열 범위를 벗어났을 때
 */
StatusCode get_z_score(const SubjectInfo *subject_info,
                       int year, int semester,
                       double raw_score,
                       SubjectZScore *z_array, int z_index)
{
    if (subject_info == NULL || z_array == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (z_index < 0 || z_index >= MAX_SUBJECT_NUM) {
        return ERROR_INDEX_OUT;
    }

    const SubjectStats *stats = find_subject_stats(subject_info, year, semester);
    if (stats == NULL) {
        return ERROR_INVALID_INPUT;
    }

    double mean  = stats->mean_raw_score;
    double stdev = stats->stdev_raw_score;
    double z = 0.0;

    if (stdev > 0.0) {
        z = (raw_score - mean) / stdev;
    } else {
        // 표준편차 0이면 분산이 없으므로 0으로 처리
        z = 0.0;
    }

    SubjectZScore *entry = &z_array[z_index];
    memset(&entry->subject, 0, sizeof(Subject));

    entry->subject.id = subject_info->id;
    strncpy(entry->subject.name, subject_info->name, NAME_LENGTH - 1);
    entry->subject.name[NAME_LENGTH - 1] = '\0';

    entry->subject.year = year;
    entry->subject.semester = semester;
    entry->subject.mean_raw_score = mean;
    entry->subject.stdev_raw_score = stdev;

    entry->z_score = z;

    return SUCCESS;
}


/**
 * @brief 내부 유틸: z_array에서 특정 과목 id의 z-score를 찾는다.
 *
 * @param z_array    학생의 z-score 배열
 * @param z_count    배열 원소 개수
 * @param subject_id 찾고자 하는 과목 id
 * @return double
 *         - 과목이 있으면 해당 z-score
 *         - 없으면 0.0
 */
static double find_z_by_id(const SubjectZScore *z_array, int z_count, int subject_id)
{
    if (z_array == NULL) return 0.0;

    for (int i = 0; i < z_count; i++) {
        if (z_array[i].subject.id == subject_id) {
            return z_array[i].z_score;
        }
    }
    return 0.0;
}


/* ============================================================
 *  TechTree 추천 관련 함수
 * ============================================================ */
/**
 * @brief 학생의 z-score 배열을 입력받아 테크트리를 추천한다.
 *
 * 점수 계산 방식:
 *   score(tree) = Σ ( user_z(subject_id) * weight )
 *
 * @param z_array          학생의 (과목, z_score) 배열
 * @param z_count          z_array에 들어있는 원소 개수
 * @param trees            테크트리 배열
 * @param tree_count       테크트리 개수
 * @param recommended_tree 추천 결과를 저장할 테크트리 포인터
 * @return StatusCode
 *         - SUCCESS: 추천 성공
 *         - ERROR_INVALID_INPUT: 인자 이상 등
 */
StatusCode recommend_techtree(const SubjectZScore *z_array,
                              int z_count,
                              const TechTree *trees,
                              int tree_count,
                              TechTree *recommended_tree)
{
    if (z_array == NULL || trees == NULL || recommended_tree == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (tree_count <= 0 || z_count < 0) {
        return ERROR_INVALID_INPUT;
    }

    double best_score = -1e100;
    int best_index = -1;

    for (int t = 0; t < tree_count; t++) {
        const TechTree *tree = &trees[t];
        if (tree->n < 0) continue;

        int subject_num = tree->n;
        if (subject_num > MAX_SUBJECT_NUM) {
            subject_num = MAX_SUBJECT_NUM;  // 방어적인 상한
        }

        double score = 0.0;

        for (int i = 0; i < subject_num; i++) {
            Subject *sub = tree->subject_arr[i];
            int weight = tree->weight[i];

            if (sub == NULL) continue;

            double user_z = find_z_by_id(z_array, z_count, sub->id);
            score += user_z * (double)weight;
        }

        if (best_index == -1 || score > best_score) {
            best_score = score;
            best_index = t;
        }
    }

    if (best_index == -1) {
        return ERROR_INVALID_INPUT;
    }

    *recommended_tree = trees[best_index];
    return SUCCESS;
}

/* ============================================================
 *  SubjectInfo 파일 저장 / 로드 함수
 * ============================================================ */

/**
 * @brief SubjectInfo 배열을 이진 파일로 저장한다.
 *
 * 파일 포맷:
 *   [int count]
 *   [SubjectInfo x count]
 *
 * @param subjects   저장할 SubjectInfo 배열 포인터
 * @param count      subjects 배열에 들어 있는 과목 개수
 * @param path       저장할 파일 경로 (예: "data/subject_infos.dat")
 * @return StatusCode
 *         - SUCCESS: 저장 성공
 *         - ERROR_INVALID_INPUT: 포인터가 NULL이거나 count가 음수일 때
 *         - ERROR_FILE_NOT_FOUND: 파일을 열지 못한 경우(경로/권한 문제 등)
 */
StatusCode save_subject_infos(const SubjectInfo *subjects, int count, const char *path)
{
    if (subjects == NULL || path == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (count < 0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = fopen(path, "wb");
    if (fp == NULL) {
        return ERROR_FILE_NOT_FOUND;
    }

    // 1) 과목 개수 먼저 기록
    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return ERROR_INVALID_INPUT;
    }

    // 2) SubjectInfo 배열 기록
    if (count > 0) {
        size_t written = fwrite(subjects, sizeof(SubjectInfo), (size_t)count, fp);
        if (written != (size_t)count) {
            fclose(fp);
            return ERROR_INVALID_INPUT;
        }
    }

    fclose(fp);
    return SUCCESS;
}

/**
 * @brief 이진 파일에서 SubjectInfo 배열을 로드한다.
 *
 * 파일 포맷:
 *   [int count]
 *   [SubjectInfo x count]
 *
 * @param subjects    데이터를 읽어올 SubjectInfo 배열 포인터
 * @param max_count   subjects 배열이 수용 가능한 최대 과목 수
 * @param out_count   실제로 읽어온 과목 수를 돌려줄 정수 포인터
 * @param path        읽어 올 파일 경로 (예: "data/subject_infos.dat")
 * @return StatusCode
 *         - SUCCESS: 로드 성공
 *         - ERROR_FILE_NOT_FOUND: 파일이 없거나 열기 실패
 *         - ERROR_INVALID_INPUT: 포인터 NULL, 파일 포맷 이상, max_count 초과 등
 */
StatusCode load_subject_infos(SubjectInfo *subjects, int max_count, int *out_count, const char *path)
{
    if (subjects == NULL || out_count == NULL || path == NULL) {
        return ERROR_INVALID_INPUT;
    }
    if (max_count <= 0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        *out_count = 0;
        return ERROR_FILE_NOT_FOUND;
    }

    int file_count = 0;
    if (fread(&file_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        *out_count = 0;
        return ERROR_INVALID_INPUT;
    }

    if (file_count < 0 || file_count > max_count) {
        fclose(fp);
        *out_count = 0;
        return ERROR_INVALID_INPUT;
    }

    if (file_count > 0) {
        size_t read = fread(subjects, sizeof(SubjectInfo), (size_t)file_count, fp);
        if (read != (size_t)file_count) {
            fclose(fp);
            *out_count = 0;
            return ERROR_INVALID_INPUT;
        }
    }

    fclose(fp);
    *out_count = file_count;
    return SUCCESS;
}