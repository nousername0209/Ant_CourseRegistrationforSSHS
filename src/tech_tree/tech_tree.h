#ifndef TECH_TREE_H
#define TECH_TREE_H

#include "../struct.h"

/* ==========================================================
 *  과목, 과목 통계, 테크트리 정보를 담을 텍스트 파일 형식 
 * ==========================================================
 *
 * 1) 과목 파일 (subjects.txt)
 *    - 빈 줄을 제외한 각 줄 형식:
 *        <id>|<name>|<credit>
 *
 *      예)
 *        44|수학 I|4
 *        45|수학 II|4
 *
 *    id     : 정수, 과목 고유 ID
 *    name   : 문자열 (예: 수학 I, 생명과학 I 등)
 *    credit : 정수, 학점
 *
 * ----------------------------------------------------------
 * 2) 과목 통계 파일 (subject_stats.txt)
 *    - 빈 줄을 제외한 각 줄 형식:
 *        <subject_id> <year> <semester> <mean> <stdev>
 *
 *      예)
 *        44 2024 1 78.5 8.2
 *        44 2024 2 75.3 7.9
 *
 *    파일을 읽을 때, 동일한 (subject_id, year, semester) 통계가
 *    이미 메모리에 있으면 그 값을 새 값으로 수정하고, 없으면 추가한다.
 *
 * ----------------------------------------------------------
 * 3) 테크트리 파일 (예: techtrees.txt)
 *    - 여러 개의 테크트리 블록이 이어진 형태:
 *
 *      <tree_id> <tree_name> <subject_count>
 *      <subject_id_1> <weight_1>
 *      <subject_id_2> <weight_2>
 *      ...
 *      <subject_id_N> <weight_N>
 *
 *      예)
 *        1 물리학 3
 *        49 5
 *        77 4
 *        53 3
 *
 *        2 생명과학 2
 *        57 5
 *        63 4
 *
 *    tree_name은 공백 없는 문자열이어야 한다.
 *
 * ==========================================================
 */

/**
 * @brief 과목 배열에서 id로 과목을 찾는다.
 * @param subjects      과목 배열 포인터
 * @param subject_count 과목 개수
 * @param subject_id    찾고자 하는 과목 ID
 * @return const SubjectInfo*
 *         - 과목을 찾으면 해당 SubjectInfo 포인터
 *         - 없으면 NULL
 */
const SubjectInfo *find_subject_by_id(SubjectInfo *subjects,
                                      int subject_count,
                                      int subject_id);

/**
 * @brief 한 과목 안에서 (year, semester)에 해당하는 통계 정보를 찾는다.
 * @param subject  통계를 조회할 과목 포인터
 * @param year     연도
 * @param semester 학기 ((1, 2, 3(계절/겨울학기)))
 * @return const SubjectStats*
 *         - 통계를 찾으면 해당 SubjectStats 포인터
 *         - 없으면 NULL
 */
const SubjectStats *find_subject_stats(const SubjectInfo *subject,
                                       int year,
                                       int semester);

/**
 * @brief 텍스트 파일에서 과목 정보를 읽어온다.
 *
 * 파일 형식: 한 줄에 한 과목
 *   <id> <name> <credit>
 *
 * @param path          텍스트 파일 경로
 * @param subjects      과목 정보를 저장할 배열
 * @param max_subjects  subjects 배열의 최대 크기
 * @param out_count     실제로 읽어온 과목 수를 돌려줄 포인터
 */
StatusCodeEnum load_subjects_from_text(const char *path,
                                   SubjectInfo *subjects,
                                   int max_subjects,
                                   int *out_count);

/**
 * @brief 텍스트 파일에서 과목 통계를 읽어와 기존 subjects에 병합한다.
 *
 * 파일 형식: 한 줄에 한 통계
 *   <subject_id> <year> <semester> <mean> <stdev>
 *
 * @param path          텍스트 파일 경로
 * @param subjects      미리 로드된 과목 배열
 * @param subject_count subjects 배열에 들어있는 과목 수
 */
StatusCodeEnum load_subject_stats_from_text(const char *path,
                                        SubjectInfo *subjects,
                                        int subject_count);

/**
 * @brief 텍스트 파일에서 테크트리 정보를 읽어온다.
 *
 * 파일 형식: 여러 블록
 *   <tree_id> <tree_name> <subject_count>
 *   <subject_id_1> <weight_1>
 *   ...
 *   <subject_id_N> <weight_N>
 *
 * @param path       텍스트 파일 경로
 * @param trees      테크트리 정보를 저장할 배열
 * @param max_trees  trees 배열의 최대 크기
 * @param out_count  실제로 읽어온 테크트리 수를 돌려줄 포인터
 */
StatusCodeEnum load_techtrees_from_text(const char *path,
                                    TechTree *trees,
                                    int max_trees,
                                    int *out_count);

/**
 * @brief 한 과목의 특정 연도/학기에 대해 학생의 원점수에 대한 z-score와 상위 퍼센트를 계산한다.
 *
 * 과목 내부에 저장된 통계(평균, 표준편차)를 사용하여, 정규분포를
 * 가정하고 z-score 및 Top x%를 계산한다.
 *
 * @param subject            과목 포인터
 * @param year               연도
 * @param semester           학기 (1~3)
 * @param raw_score          학생의 원점수
 * @param out_z              계산된 z-score를 돌려줄 포인터
 * @param out_percentile_top 상위 퍼센트(Top x%)를 돌려줄 포인터
 */
StatusCodeEnum compute_z_and_percentile(const SubjectInfo *subject,
                                    int year,
                                    int semester,
                                    double raw_score,
                                    double *out_z,
                                    double *out_percentile_top);

/**
 * @brief 학생 성적 하나를 scores 배열에 추가하면서 z-score/상위 퍼센트를 계산한다.
 *
 * 내부적으로 compute_z_and_percentile을 호출하여 통계를 이용해
 * z-score와 상위 퍼센트를 계산한 뒤 StudentScore를 채워 넣는다.
 *
 * @param scores        성적 배열
 * @param max_scores    scores 배열의 최대 크기
 * @param inout_count   현재 성적 개수(입력)와 추가 후 개수(출력)를 담는 포인터
 * @param user_id       학생 ID
 * @param subject_id    과목 ID
 * @param year          연도
 * @param semester      학기 (1~3)
 * @param raw_score     원점수
 * @param subjects      과목 배열 (통계 포함)
 * @param subject_count 과목 개수
 */
StatusCodeEnum add_student_score(StudentScore *scores,
                             int max_scores,
                             int *inout_count,
                             int user_id,
                             int subject_id,
                             int year,
                             int semester,
                             double raw_score,
                             const SubjectInfo *subjects,
                             int subject_count);

/**
 * @brief 학생에게 적합한 테크트리들을 점수 순으로 정렬해 순위 목록을 돌려준다.
 * 
 * 각 테크트리에 대해 다음과 같이 점수를 계산한다.
 *   - 테크트리의 모든 노드(subject_id, weight)에 대해,
 *     해당 학생(user_id)의 성적들 중 그 과목의 가장 좋은
 *     (가장 작은) 상위 퍼센트(percentile_top)를 찾는다.
 *   - (100 - percentile_top) * weight 를 누적하여 테크트리 점수를 만든다.
 * 
 * 사용 가능한(해당 트리에 반영할 성적이 하나라도 있는) 테크트리들만 골라
 * 점수 내림차순으로 정렬하여 out_indices / out_scores에 채워준다.
 *
 * @param trees         테크트리 배열
 * @param tree_count    테크트리 개수
 * @param scores        학생 성적 배열
 * @param score_count   성적 개수
 * @param user_id       대상 학생 ID
 * @param out_indices   정렬된 테크트리 인덱스를 담을 배열 (크기 >= max_out)
 * @param out_scores    각 테크트리 점수를 담을 배열 (크기 >= max_out, NULL 가능)
 * @param max_out       out_indices / out_scores에 담을 수 있는 최대 개수
 * @param out_returned  실제로 반환된 테크트리 개수를 돌려줄 포인터
 */
StatusCodeEnum rank_techtrees(const TechTree *trees,
                          int tree_count,
                          const StudentScore *scores,
                          int score_count,
                          int user_id,
                          int *out_indices,
                          double *out_scores,
                          int max_out,
                          int *out_returned);


#endif 
