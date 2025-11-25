//
// Created by 김주환 on 25. 11. 9.
//
// modified by 장민준 on 2025.11.23. find_subject_stats, add_subject_stats, has_prereq 함수 추가
// modified by 장민준 on 2025.11.24. 

#ifndef RECOMMENDER_H
#define RECOMMENDER_H
#include "../struct.h"

/**
 * @brief SubjectInfo 안에서 특정 year/semester에 해당하는 원점수 평균/표준편차 통계(SubjectStats)를 찾는다.
 *
 * @param subject_info  통계를 찾을 과목 SubjectInfo 구조체 포인터
 * @param year          찾고자 하는 연도 (예: 2025)
 * @param semester      학기 (1, 2, 3(계절/겨울))
 * @return const SubjectStats*
 *         - 찾으면 해당 SubjectStats의 포인터
 *         - 없으면 NULL
 */
const SubjectStats *find_subject_stats(const SubjectInfo *subject_info, int year, int semester);


/**
 * @brief SubjectInfo에 새로운 통계(SubjectStats)를 하나 추가한다.
 *
 * @param subject_info    통계를 추가할 과목 SubjectInfo 구조체 포인터
 * @param year            연도
 * @param semester        학기
 * @param mean_raw_score  해당 연도/학기의 원점수 평균
 * @param stdev_raw_score 해당 연도/학기의 원점수 표준편차
 * @return StatusCodeEnum
 *         - SUCCESS: 추가 성공
 *         - ERROR_INVALID_INPUT: subject_info가 NULL일 때
 *         - ERROR_INDEX_OUT: MAX_SUBJECT_STATS를 초과했을 때 등
 */
StatusCodeEnum add_subject_stats(SubjectInfo *subject_info, int year, int semester, double mean_raw_score, double stdev_raw_score);

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
int meets_prereq(const SubjectInfo *subject_info, const int taken_ids[], int taken_count);

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
 *         - ERROR_INVALID_INPUT: subject_info가 NULL이거나 통계를 찾지 못했을 때 등
 */
StatusCodeEnum get_z_score(const SubjectInfo *subject_info, int year, int semester, double raw_score, SubjectZScore *z_array, int z_index);

/**
 * @brief 학생의 z-score 배열을 입력받아 테크트리를 추천한다.
 *
 * @param z_array          학생의 (과목, z_score) 배열
 * @param z_count          z_array에 들어있는 원소 개수
 * @param trees            테크트리 배열
 * @param tree_count       테크트리 개수
 * @param recommended_tree 추천 결과를 저장할 테크트리 포인터
 * @return StatusCodeEnum
 *         - SUCCESS: 추천 성공
 *         - ERROR_INVALID_INPUT: 인자 이상 등
 */
StatusCodeEnum recommend_techtree(const SubjectZScore *z_array, int z_count, const TechTree *trees, int tree_count, TechTree *recommended_tree);

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
 * @return StatusCodeEnum
 *         - SUCCESS: 저장 성공
 *         - ERROR_INVALID_INPUT: 포인터가 NULL이거나 count가 음수일 때
 *         - ERROR_FILE_NOT_FOUND: 파일을 열지 못한 경우(경로/권한 문제 등)
 */
StatusCodeEnum save_subject_infos(const SubjectInfo *subjects, int count, const char *path);

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
 * @return StatusCodeEnum
 *         - SUCCESS: 로드 성공
 *         - ERROR_FILE_NOT_FOUND: 파일이 없거나 열기 실패
 *         - ERROR_INVALID_INPUT: 포인터 NULL, 파일 포맷 이상, max_count 초과 등
 */
StatusCodeEnum load_subject_infos(SubjectInfo *subjects, int max_count, int *out_count, const char *path);

#endif