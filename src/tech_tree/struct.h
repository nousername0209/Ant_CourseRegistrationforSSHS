#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>

/* 이름(과목명, 테크트리명 등) 최대 길이 */
#define NAME_LENGTH            64

/* 한 과목에 저장 가능한 성적 통계(연도/학기별) 최대 개수 */
#define MAX_SUBJECT_STATS      16

/* 전체 과목 최대 개수 */
#define MAX_SUBJECTS           128

/* 한 테크트리에 들어갈 수 있는 과목 최대 개수 */
#define MAX_TREE_SUBJECTS      32

/* 전체 테크트리 최대 개수 */
#define MAX_TECH_TREES         32

/* 전체 학생 성적 기록 최대 개수 */
#define MAX_SCORES             512


/* 함수 호출 결과를 나타내는 공통 상태 코드 */
typedef enum {
    STATUS_OK = 0,                 /* 정상 동작 */
    STATUS_ERROR_INVALID_INPUT,    /* 잘못된 인자(포인터 NULL, 범위 오류 등) */
    STATUS_ERROR_FILE,             /* 파일 열기/읽기/쓰기 실패 */
    STATUS_ERROR_PARSE,            /* 텍스트 파싱 실패 */
    STATUS_ERROR_NOT_FOUND,        /* 원하는 데이터를 찾지 못함 */
    STATUS_ERROR_CAPACITY          /* 배열 용량 초과 */
} StatusCode;


/* 한 과목의 특정 학기(연도, 학기)의 원점수에 대한 통계 정보 */
typedef struct {
    int year;       /* 연도 */
    int semester;   /* 학기 (1, 2, 3(계절/겨울학기)) */
    double mean;    /* 원점수의 평균 */
    double stdev;   /* 원점수의 표준편차 (0보다 커야 함) */
} SubjectStats;


/* 과목 기본 정보 + 선수과목 + 통계 */
typedef struct {
    int id;                             /* 과목 고유 ID */
    char name[NAME_LENGTH];            /* 과목 이름 */
    int credit;                        /* 학점 */

    int prereq_count;                  /* 선수 과목 수 */
    int prereq_ids[8];                 /* 선수 과목 ID 목록 (최대 8개) */

    int stats_count;                   /* 통계 정보 개수 */
    SubjectStats stats[MAX_SUBJECT_STATS];  /* 학기별 통계 정보 배열 */
} SubjectInfo;

/* 테크트리 안의 한 노드(과목 + 가중치) */
typedef struct {
    int subject_id;     /* 과목 ID */
    int weight;         /* 중요도 가중치 (정수, 클수록 중요) */
} TreeSubject;

/* 테크트리 정의 */
typedef struct {
    int id;                             /* 테크트리 ID */
    char name[NAME_LENGTH];            /* 테크트리 이름 */

    int subject_count;                 /* 이 테크트리에 포함된 과목 수 */
    TreeSubject subjects[MAX_TREE_SUBJECTS];  /* 노드 배열 */
} TechTree;

/* 한 학생의 한 과목 성적 기록 */
typedef struct {
    int user_id;           /* 학생 ID (외부 시스템에서 관리) */
    int subject_id;        /* 과목 ID */
    int year;              /* 연도 */
    int semester;          /* 학기 (1~3) */

    double raw_score;      /* 원점수 */
    double z_score;        /* z-score ((원점수-평균)/표준편차) */
    double percentile_top; /* 상위 퍼센트 (Top x%) - 작을수록 성적이 좋은 것 */
} StudentScore;

#endif 
