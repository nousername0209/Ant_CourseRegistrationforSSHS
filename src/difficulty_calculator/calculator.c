//
// Created by 김주환 on 25. 11. 9.
//

#include "../struct.h" 
#include "../login/login.h"
#include "calculator.h"

/**
 * preprocess_load
 * dir_path에 있는 데이터셋을 바탕으로 Load(단일 과목 난이도)를 로드한다.
 */
StatusCode preprocess_load(double (*Load)[MAX_SUBJECT_NUM]) {
    char dir_path[PATH_LENGTH] = "../../dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 20];
    char data_path[PATH_LENGTH + 40];
    FILE *fp = NULL;
    DIR *dir = NULL;
    int count[MAX_SUBJECT_NUM];

    for (int i = 0; i < MAX_SUBJECT_NUM; i++) {
        (*Load)[i] = 0.0;
        count[i]=0;
    }   

    sprintf(data_path, "%s/data");
    dir = opendir(data_path);
    if(dir==NULL)
        return ERROR_FILE_NOT_FOUND;
    
    struct dirent *ent = NULL;
    while ((ent=readdir(dir))!=NULL) {
        sprintf(file_path, "%s/%s", dir_path, ent->d_name);
        fp = fopen(file_path, "r");
        if (fp == NULL)
            return ERROR_FILE_NOT_FOUND;
        int n, tmp;
        fscanf(fp, "%d %d", &n, &tmp);
        for(int i = 0 ; i < n ; i++){
            int subject, hard;
            fscanf(fp, "%d %d", &subject, &hard);
            (*Load)[subject]+=hard;
            count[subject]++;
        }
        fclose(fp);
    }
    closedir(dir);

    for(int i = 0 ; i < MAX_SUBJECT_NUM ; i++){
        (*Load)[i]/=count[i];
    }

    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "w+b");
    fwrite(*Load, sizeof(int), MAX_SUBJECT_NUM, fp);
    fclose(fp);

    return SUCCESS;
}

/**
 * preprocess_synergy
 * dir_path에 있는 데이터셋을 바탕으로 Synergy(과목 간 시너지/교차 난이도)를 로드한다
 */
StatusCode preprocess_synergy(double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM]) {
    char dir_path[PATH_LENGTH] = "../../dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 20];
    char data_path[PATH_LENGTH + 40];
    FILE *fp = NULL;
    DIR *dir = NULL;
    int subject[MAX_SUBJECT_NUM];
    int Load[MAX_SUBJECT_NUM];

    for (int i = 0; i < MAX_SUBJECT_NUM; i++) {
        for (int j = 0; j < MAX_SUBJECT_NUM; j++) {
            (*Synergy)[i][j] = 0.0;
        }
    }

    int id1, id2;
    double val;

    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp ==NULL)
        return ERROR_FILE_NOT_FOUND;
    fread(Load, sizeof(int), MAX_SUBJECT_NUM, fp);
    fclose(fp);


    sprintf(data_path, "%s/data", dir_path);
    dir = opendir(data_path);
    if(dir==NULL)
        return ERROR_FILE_NOT_FOUND;
    
    struct dirent *ent = NULL;
    while ((ent=readdir(dir))!=NULL) {
        sprintf(file_path, "%s/%s", dir_path, ent->d_name);
        fp = fopen(file_path, "r");
        if (fp == NULL)
            return ERROR_FILE_NOT_FOUND;
        int n, total_hard;
        double expect_hard=0, average_synergy;
        fscanf(fp, "%d %d", &n, &total_hard);
        for(int i=0;i<n;i++){
            int hard;
            fscanf(fp, "%d %d", &subject[i], &hard);
            expect_hard += hard;
        }
        fclose(fp);
        expect_hard /= n;
        average_synergy =(total_hard-expect_hard)*2/(n*(n+1));

        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(i==j)
                    continue;
                (*Synergy)[subject[i]][subject[j]]+=average_synergy;
            }
        }
    }
    for(int i=0;i<MAX_SUBJECT_NUM;i++)
        (*Synergy)[i][i] = Load[i];
    fclose(fp);

    sprintf(file_path, "%s/synergy.dat", dir_path);
    fp = fopen(file_path, "w+b");
    fwrite(*Synergy, sizeof(int), MAX_SUBJECT_NUM * MAX_SUBJECT_NUM, fp);
    fclose(fp);

    return SUCCESS;
}

/**
 * calculate_difficulty
 * 주어진 Load와 Synergy를 바탕으로 TimeTable의 난이도 요소를 계산한다.
 * 가장 Load가 큰 과목(argmax_load)과 Synergy가 가장 높은 과목 쌍(argmax_synergy)을 찾는다.
 */
StatusCode calculate_difficulty(const TimeTable* table,
                                Subject *argmax_load,
                                Subject (*argmax_synergy)[2],
                                double *total_difficulty) {
    if (table == NULL || argmax_load == NULL || argmax_synergy == NULL || table->n ==0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = NULL;
    char dir_path[PATH_LENGTH] = "../../dataset/difficulty_calulator";
    char file_path[PATH_LENGTH];

    double Load[MAX_SUBJECT_NUM];
    double Synergy[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];
    *total_difficulty = 0;
    
    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "rb");
    fread(Load, sizeof(double), MAX_SUBJECT_NUM, fp);
    fclose(fp);

    sprintf(file_path, "%s/synergy.dat", dir_path);
    fp = fopen(file_path, "rb");
    fread(Synergy, sizeof(double), MAX_SUBJECT_NUM, fp);
    fclose(fp);


    *argmax_load = *(table->subjects[0]);
    for (int i = 0; i < table->n; i++) {
        Subject *cur_subject = table->subjects[i];
        if(cur_subject == NULL ||
           cur_subject->id < 0 || cur_subject->id >=MAX_SUBJECT_NUM )
            return ERROR_INVALID_INPUT;
        
        if(Load[cur_subject->id] > Load[argmax_load->id])
            *argmax_load = *cur_subject;
        *total_difficulty += Load[cur_subject->id];
    }

    if(table->n==1){
        (*argmax_synergy)[0] = *(table->subjects[0]);
        (*argmax_synergy)[1] = *(table->subjects[0]);
    }
    else{
        (*argmax_synergy)[0] = *(table->subjects[0]);
        (*argmax_synergy)[1] = *(table->subjects[1]);
        for(int i = 0 ; i < table->n ; i++){
            for(int j = i+1 ; j< table->n ; j++){
                if(Synergy[table->subjects[i]->id][table->subjects[j]->id] >
                   Synergy[(*argmax_synergy)[0].id][(*argmax_synergy)[1].id]){
                    (*argmax_synergy)[0] = *(table->subjects[i]);
                    (*argmax_synergy)[1] = *(table->subjects[j]);
                }
                *total_difficulty += Synergy[table->subjects[i]->id][table->subjects[j]->id];
            }
        }
    }
    return SUCCESS;
}

/**
 * add_difficulty_db
 * 지정된 path에 difficulty 데이터베이스를 추가한다.
 * 필요한 정보(ID, 값 등)는 함수 내부에서 사용자로부터 입력받는다.
 */
StatusCode add_difficulty_db() {
    char dir_path[PATH_LENGTH] = "../../dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 20];
    int choice;
    
    printf("데이터 추가 모드 선택 (1: Load, 2: Synergy): ");
    if (scanf("%d", &choice) != 1) return ERROR_INVALID_INPUT;

    if (choice == 1) {
        // Load 데이터 추가
        snprintf(file_path, sizeof(file_path), "%s/load.txt", path);
        FILE *fp = fopen(file_path, "a"); // append 모드로 열기
        if (fp == NULL) return ERROR_FILE_NOT_FOUND;

        int id;
        double val;
        printf("추가할 과목 ID와 Load 값을 입력하세요 (예: 3 10.5): ");
        if (scanf("%d %lf", &id, &val) == 2) {
            fprintf(fp, "%d %lf\n", id, val);
            printf("Load 데이터가 추가되었습니다.\n");
        } else {
            fclose(fp);
            return ERROR_INVALID_INPUT;
        }
        fclose(fp);

    } else if (choice == 2) {
        // Synergy 데이터 추가
        snprintf(file_path, sizeof(file_path), "%s/synergy.txt", path);
        FILE *fp = fopen(file_path, "a"); // append 모드로 열기
        if (fp == NULL) return ERROR_FILE_NOT_FOUND;

        int id1, id2;
        double val;
        printf("추가할 두 과목 ID와 Synergy 값을 입력하세요 (예: 3 5 2.0): ");
        if (scanf("%d %d %lf", &id1, &id2, &val) == 3) {
            fprintf(fp, "%d %d %lf\n", id1, id2, val);
            printf("Synergy 데이터가 추가되었습니다.\n");
        } else {
            fclose(fp);
            return ERROR_INVALID_INPUT;
        }
        fclose(fp);

    } else {
        printf("잘못된 선택입니다.\n");
        return ERROR_INVALID_INPUT;
    }

    return SUCCESS;
}