#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "calculator.h"


// // 테스트용 매크로
// #define TEST_DIR "../../dataset/difficulty_calculator"
// #define DATA_DIR "../../dataset/difficulty_calculator/data"

// // 디렉토리 생성 헬퍼 함수
// void create_directories() {
// #ifdef _WIN32
//     mkdir("../../dataset");
//     mkdir(TEST_DIR);
//     mkdir(DATA_DIR);
// #else
//     mkdir("../../dataset", 0777);
//     mkdir(TEST_DIR, 0777);
//     mkdir(DATA_DIR, 0777);
// #endif
// }

// 테스트 데이터 생성 함수
void create_dummy_data() {
    printf("[Debug] Creating dummy data files...\n");
    
    // 1. data/sample1.txt 생성 (과목 2개, 난이도 합계 20)
    // 포맷: [과목수] [총난이도] \n [ID] [난이도] ...
    FILE *fp = fopen("/sample1.txt", "w");
    if (fp) {
        fprintf(fp, "2 20\n");
        fprintf(fp, "1 10\n"); // ID 1, 난이도 10
        fprintf(fp, "2 10\n"); // ID 2, 난이도 10
        fclose(fp);
    }

    // 2. data/sample2.txt 생성 (과목 3개, 난이도 합계 45)
    fp = fopen("/sample2.txt", "w");
    if (fp) {
        fprintf(fp, "3 45\n");
        fprintf(fp, "1 20\n"); // ID 1, 난이도 20 (sample1과 합치면 ID 1의 총합은 30)
        fprintf(fp, "3 15\n"); // ID 3, 난이도 15
        fprintf(fp, "5 10\n"); // ID 5, 난이도 10
        fclose(fp);
    }
    printf("[Debug] Dummy data created successfully.\n");
}

void print_subject(Subject *s) {
    if (s) printf("ID: %d, Name: %s\n", s->id, s->name);
    else printf("(NULL)\n");
}

int main() {
    // 0. 환경 설정
    StatusCode status;
    // status = add_difficulty_db();
    // if (status == SUCCESS){
    //     printf("[SUCCESS] database added\n");
    // } else{
    //     printf("[FAIL] database adding failed with code %d\n", status);
    // }
    
    double Load[MAX_SUBJECT_NUM], Synergy[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];

    // ==========================================
    // TEST 1: Preprocess Load
    // ==========================================
    printf("\n=== TEST 1: Preprocess Load ===\n");
    status = preprocess_load(&Load);
    if (status == SUCCESS) {
        printf("[SUCCESS] preprocess_load executed.\n");
        // 검증: ID 1의 평균 난이도 확인
        // ID 1: (10 + 20) / 2 = 15.0
        printf("Subject ID 1 Load (Expected: 15.0): %.2f\n", Load[1]);
        printf("Subject ID 2 Load (Expected: 10.0): %.2f\n", Load[2]);
    } else {
        printf("[FAIL] preprocess_load failed with code %d\n", status);
    }

    // ==========================================
    // TEST 2: Preprocess Synergy
    // ==========================================
    printf("\n=== TEST 2: Preprocess Synergy ===\n");
    status = preprocess_synergy(&Synergy);
    if (status == SUCCESS) {
        printf("[SUCCESS] preprocess_synergy executed.\n");
        // 검증: 대각 성분은 Load 값이어야 함
        printf("Synergy[1][1] (Expected: 15.0): %.2f\n", Synergy[1][1]);
        printf("Synergy[1][2] (Expected approx 4.00): %.2f\n", Synergy[1][2]);
        printf("Synergy[1][3] (Expected approx 5.00): %.2f\n", Synergy[1][3]);
    } else {
        printf("[FAIL] preprocess_synergy failed with code %d\n", status);
    }

    // ==========================================
    // TEST 3: Calculate Difficulty
    // ==========================================
    printf("\n=== TEST 3: Calculate Difficulty ===\n");
    
    // 가상의 시간표 생성
    TimeTable table;
    Subject s1, s2, s3;
    
    // 과목 정보 설정
    s1.id = 1; strcpy(s1.name, "Math");
    s2.id = 2; strcpy(s2.name, "English");
    s3.id = 3; strcpy(s3.name, "Science");

    // 시간표에 과목 추가
    table.n = 3;
    table.subjects[0] = &s1;
    table.subjects[1] = &s2;
    table.subjects[2] = &s3;

    Subject argmax_load;
    Subject argmax_synergy[2];
    double total_difficulty;

    status = calculate_difficulty(&table);

    if (status == SUCCESS) {
        printf("[SUCCESS] calculate_difficulty executed.\n");

        // [총 난이도 계산]
        // 1. Load Sum: 15(ID1) + 10(ID2) + 15(ID3) = 40.0
        // 2. Synergy Sum:
        //    (1,2): 4.0
        //    (1,3): 5.0
        //    (2,3): 0.0 (데이터 없음)
        //    Total Synergy = 9.0
        // 3. Total Difficulty = 40.0 + 9.0 = 49.0

        printf("Total Difficult(expected 49.00): %.2f\n", total_difficulty);
        
        printf("Max Load Subject: ");
        print_subject(&argmax_load);
        
        printf("Max Synergy Pair(expected Math & Science): \n");
        print_subject(&argmax_synergy[0]);
        print_subject(&argmax_synergy[1]);
        // Synergy 쌍 비교: 
        // (1,2) -> 3.33
        // (1,3) -> 5.0  (Sample 2에서 옴)
        // (2,3) -> 0.0  (같이 들은 적 없음)
        // 예상: (1, 3) 쌍이 나와야 함
    } else {
        printf("[FAIL] calculate_difficulty failed with code %d\n", status);
    }


    return 0;
}