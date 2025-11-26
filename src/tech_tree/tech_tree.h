// 이연지 작업

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
 *        1 물리 3
 *        49 5
 *        77 4
 *        53 3
 *
 *        2 생물 2
 *        57 5
 *        63 4
 *
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
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 과목 정보를 모두 읽음
 *         - ERROR_INVALID_INPUT: 인자가 잘못된 경우
 *         - ERROR_FILE_NOT_FOUND: 파일을 열 수 없는 경우
 *         - STATUS_ERROR_PARSE: 파싱 오류 발생
 *         - ERROR_INDEX_OUT: 배열 범위를 초과한 경우
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
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 통계를 병합함
 *         - ERROR_INVALID_INPUT: 인자가 잘못된 경우
 *         - ERROR_FILE_NOT_FOUND: 파일을 열 수 없는 경우
 *         - STATUS_ERROR_PARSE: 파싱 오류
 *         - ERROR_INDEX_OUT: 과목별 통계 배열 범위 초과
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
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 테크트리를 읽음
 *         - ERROR_INVALID_INPUT: 인자가 잘못된 경우
 *         - ERROR_FILE_NOT_FOUND: 파일을 열 수 없는 경우
 *         - STATUS_ERROR_PARSE: 파싱 오류
 *         - ERROR_INDEX_OUT: 배열 범위 초과
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
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 계산 완료
 *         - ERROR_INVALID_INPUT: 인자가 잘못된 경우 또는 표준편차가 0 이하인 경우
 *         - ERROR_FILE_NOT_FOUND: 해당 (year, semester)에 대한 통계가 없는 경우
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
 * @return StatusCodeEnum
 *         - SUCCESS: 성적 추가 및 계산 성공
 *         - ERROR_INVALID_INPUT: 인자 오류
 *         - ERROR_INDEX_OUT: 성적 배열이 가득 찬 경우
 *         - ERROR_FILE_NOT_FOUND: 해당 과목/통계 정보를 찾을 수 없는 경우
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
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 순위 계산 완료
 *         - ERROR_INVALID_INPUT: 인자 오류
 *         - ERROR_FILE_NOT_FOUND: 활용 가능한 성적이 하나도 없는 경우
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

/* ==========================================================
 *  성적 입력 / 테크트리 추천 UI 관련 함수
 * ========================================================== */

/**
 * @brief 주어진 시간표(TimeTable)의 과목들에 대해 원점수(raw_score) 입력 팝업 UI를 그린다.
 *
 * output.c의 popup_input_difficulty와 유사한 방식으로 콘솔 UI를 출력하며,
 * 각 과목 이름과 현재 입력된 원점수를 표시한다.
 *
 * @param t          성적을 입력할 과목 목록 (TimeTable)
 * @param raw_scores 각 과목에 대응하는 원점수 배열
 * @param current_idx 현재 선택된 입력 필드 인덱스 (0~n-1)
 * @param box_w      팝업 박스 너비
 * @param box_h      팝업 박스 높이
 * @param start_x    팝업 시작 X 좌표
 * @param start_y    팝업 시작 Y 좌표
 */
void draw_score_input_popup(TimeTable *t,
                            double raw_scores[],
                            int current_idx,
                            int scroll_offset,
                            int box_w,
                            int box_h,
                            int start_x,
                            int start_y);

/**
 * @brief 난이도(성적) 입력 값이 유효한지 검사한다 (0~100 사이 실수값을 정수*10 형태로 받음).
 *
 * 내부적으로 문자열을 정수로 파싱하여 0 이상 1000 이하인지 검사한다.
 *
 * @param buf 입력 문자열 버퍼
 * @return int
 *         - 1: 유효한 입력
 *         - 0: 유효하지 않은 입력
 */
int is_valid_score_input(const char *buf);

/**
 * @brief 성적 입력 팝업 UI를 실행하고 입력된 데이터를 raw_scores에 채운다.
 *
 * TimeTable에 포함된 각 과목에 대해 원점수(0~100)를 입력받는 콘솔 UI를 띄운다.
 * ENTER, 방향키 등을 사용해 입력 필드를 이동하고, "저장"을 선택하면 점수 입력을 완료한다.
 * (내부에서 g_scores/g_score_count에 add_student_score를 호출하는 구현을 가질 수 있다.)
 *
 * @param t          성적을 입력할 과목 목록 (TimeTable)
 * @param raw_scores 입력된 원점수(0~100)를 저장할 배열 (크기 >= t->n)
 * @param user_id    현재 사용자 ID
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 입력 완료
 *         - ERROR_CANCEL: ESC 등으로 입력을 취소한 경우
 *         - ERROR_INVALID_INPUT: 내부 입력 처리 중 오류
 */
StatusCodeEnum popup_input_scores(TimeTable *t,
                                  double raw_scores[],
                                  int user_id);

/**
 * @brief 사용자 정보와 시간표를 바탕으로 성적 입력 및 StudentScore 배열 갱신을 수행한다.
 *
 * user.current_sem - 1 학기까지의 기이수 과목을 모두 모아 하나의 임시 TimeTable에 담고,
 * popup_input_scores를 이용해 각 과목의 원점수를 입력받는다.
 * 이후 scores 배열에 학생의 성적 정보를 추가/갱신한다.
 *
 * @param user        사용자 정보 포인터 (시간표 정보 포함)
 * @param scores      성적 배열
 * @param score_count 성적 개수의 포인터 (입력: 현재 개수, 출력: 갱신 후 개수)
 * @param max_scores  scores 배열의 최대 크기
 * @param student_id  대상 학생 ID
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 성적 입력 및 갱신 완료
 *         - ERROR_CANCEL: 사용자가 입력을 취소한 경우
 *         - ERROR_INVALID_INPUT: 인자 오류 또는 내부 처리 오류
 */
StatusCodeEnum run_score_input_system(User *user,
                                      StudentScore *scores,
                                      int *score_count,
                                      int max_scores,
                                      int student_id);

/**
 * @brief 테크트리 추천 결과를 콘솔 UI로 출력하는 함수.
 *
 * rank_techtrees를 이용해 주어진 학생(user_id)에 대한 테크트리 점수를 계산한 뒤,
 * 순위 목록과 TOP 1 테크트리의 상세 기여도 정보를 콘솔에 출력한다.
 * ESC 키 입력 시까지 결과 화면을 유지한다.
 *
 * @param trees         테크트리 배열
 * @param tree_count    테크트리 개수
 * @param scores        학생 성적 배열
 * @param score_count   성적 개수
 * @param subjects      과목 정보 배열
 * @param subject_count 과목 개수
 * @param user_id       대상 학생 ID
 * @return StatusCodeEnum
 *         - SUCCESS: 정상적으로 추천 결과 출력
 *         - ERROR_FILE_NOT_FOUND: 활용 가능한 성적/통계가 부족한 경우
 *         - ERROR_INVALID_INPUT: 인자 오류
 *         - 기타 rank_techtrees에서 반환된 오류 코드
 */
StatusCodeEnum run_recommendation_ui(const TechTree *trees,
                                     int tree_count,
                                     const StudentScore *scores,
                                     int score_count,
                                     const SubjectInfo *subjects,
                                     int subject_count,
                                     int user_id);

/**
 * @brief 테크트리 시스템의 메인 진입 함수.
 *
 * 사용자 데이터 및 과목/통계/테크트리 정보를 로드한 뒤,
 * 성적 입력 UI를 실행하고, 그 결과를 바탕으로 테크트리 추천 UI를 호출한다.
 *
 * @param student_id 대상 학생 ID
 */
void run_tech_tree(int student_id);

#endif /* TECH_TREE_H */
