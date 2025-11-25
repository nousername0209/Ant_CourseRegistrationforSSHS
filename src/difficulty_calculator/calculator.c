//
// Created by ï¿??ì£¼í™˜ on 25. 11. 9.
//

#include "../struct.h" 
// #include "../login/login.h"
#include "calculator.h"

/**
 * preprocess_load
 * dir_path?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ ë°”íƒ•?ï¿½ï¿½ï¿?? Load(?ï¿½ï¿½?ï¿½ï¿½ ê³¼ëª© ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½)ï¿?? ë¡œë“œ?ï¿½ï¿½?ï¿½ï¿½.
 */
StatusCode preprocess_load(double (*Load)[MAX_SUBJECT_NUM]) {
    char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 20];
    char data_path[PATH_LENGTH + 40];
    FILE *fp = NULL;
    DIR *dir = NULL;
    int count[MAX_SUBJECT_NUM];

    for (int i = 0; i < MAX_SUBJECT_NUM; i++) {
        (*Load)[i] = 0.0;
        count[i]=0;
    }

    sprintf(data_path, "%s/data", dir_path);
    dir = opendir(data_path);
    if(dir==NULL)
        return ERROR_FILE_NOT_FOUND;
    
    struct dirent *ent = NULL;
    while ((ent=readdir(dir))!=NULL) {
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        sprintf(file_path, "%s/%s", data_path, ent->d_name);
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
        if (count[i]==0)
            continue;
        (*Load)[i]/=count[i];
    }

    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "w+b");
    fwrite(*Load, sizeof(double), MAX_SUBJECT_NUM, fp);
    fclose(fp);

    return SUCCESS;
}

/**
 * preprocess_synergy
 * dir_path?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ ë°”íƒ•?ï¿½ï¿½ï¿?? Synergy(ê³¼ëª© ï¿?? ?ï¿½ï¿½?ï¿½ï¿½ï¿??/êµì°¨ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½)ï¿?? ë¡œë“œ?ï¿½ï¿½?ï¿½ï¿½
 */
StatusCode preprocess_synergy(double (*Synergy)[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM]) {
    char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
    char file_path[PATH_LENGTH + 20];
    char data_path[PATH_LENGTH + 40];
    FILE *fp = NULL;
    DIR *dir = NULL;
    int subject[MAX_SUBJECT_NUM];
    double Load[MAX_SUBJECT_NUM];
    int count[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];

    int id1, id2;
    double val;

    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "rb");
    if(fp ==NULL)
        return ERROR_FILE_NOT_FOUND;
    fread(Load, sizeof(double), MAX_SUBJECT_NUM, fp);
    fclose(fp);

    sprintf(data_path, "%s/data", dir_path);
    dir = opendir(data_path);
    if(dir==NULL)
        return ERROR_FILE_NOT_FOUND;

    for(int i=0;i<MAX_SUBJECT_NUM;i++){
        for(int j=0;j<MAX_SUBJECT_NUM;j++){
            (*Synergy)[i][j]=0.0;
            count[i][j]=0;
        }
    }
    
    struct dirent *ent = NULL;
    while ((ent=readdir(dir))!=NULL) {
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        sprintf(file_path, "%s/%s", data_path, ent->d_name);
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
        average_synergy =(total_hard-expect_hard)*2/(n*(n-1));

        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(i==j)
                    continue;
                (*Synergy)[subject[i]][subject[j]]+=average_synergy;
                count[subject[i]][subject[j]]++;
            }
        }
    }
    for(int i=0;i<MAX_SUBJECT_NUM;i++){
        for(int j=0;j<MAX_SUBJECT_NUM;j++){
            if (count[i][j]==0)
                continue;
            (*Synergy)[i][j]/=count[i][j];
        }
    }
    for(int i=0;i<MAX_SUBJECT_NUM;i++)
        (*Synergy)[i][i] = Load[i];
    fclose(fp);

    sprintf(file_path, "%s/synergy.dat", dir_path);
    fp = fopen(file_path, "w+b");
    fwrite(*Synergy, sizeof(double), MAX_SUBJECT_NUM * MAX_SUBJECT_NUM, fp);
    fclose(fp);

    return SUCCESS;
}

/**
 * calculate_difficulty
 * ì£¼ì–´ï¿?? Load??? Synergyï¿?? ë°”íƒ•?ï¿½ï¿½ï¿?? TimeTable?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ï¿?? ê³„ì‚°?ï¿½ï¿½?ï¿½ï¿½.
 * ï¿???ï¿½ï¿½ Loadï¿?? ?ï¿½ï¿½ ê³¼ëª©(argmax_load)ï¿?? Synergyï¿?? ï¿???ï¿½ï¿½ ?ï¿½ï¿½??? ê³¼ëª© ?ï¿½ï¿½(argmax_synergy)?ï¿½ï¿½ ì°¾ëŠ”?ï¿½ï¿½.
 */
StatusCode calculate_difficulty(TimeTable* table) {
    if (table == NULL || table->n ==0) {
        return ERROR_INVALID_INPUT;
    }

    FILE *fp = NULL;
    char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
    char data_path[PATH_LENGTH + 40];
    char file_path[PATH_LENGTH + 40];

    double Load[MAX_SUBJECT_NUM];
    double Synergy[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];
    table->difficulty = 0;
    table->argmax_load = (Subject *)malloc(sizeof(Subject));
    table->argmax_synergy = (Subject (*)[2])malloc(sizeof(Subject)*2);

    sprintf(data_path, "%s/data", dir_path);
    
    sprintf(file_path, "%s/load.dat", dir_path);
    fp = fopen(file_path, "rb");
    if (fp==NULL){
        return ERROR_FILE_NOT_FOUND;
    }
    fread(Load, sizeof(double), MAX_SUBJECT_NUM, fp);
    fclose(fp);

    sprintf(file_path, "%s/synergy.dat", dir_path);
    fp = fopen(file_path, "rb");
    if (fp==NULL){
        return ERROR_FILE_NOT_FOUND;
    }
    fread(Synergy, sizeof(double), MAX_SUBJECT_NUM*MAX_SUBJECT_NUM, fp);
    fclose(fp);

    *(table->argmax_load) = *(table->subjects[0]);
    for (int i = 0; i < table->n; i++) {
        Subject *cur_subject = table->subjects[i];
        if(cur_subject == NULL ||
           cur_subject->id < 0 || cur_subject->id >=MAX_SUBJECT_NUM )
            return ERROR_INVALID_INPUT;
        
        if(Load[cur_subject->id] > Load[table->argmax_load->id])
            *(table->argmax_load) = *cur_subject;
        table->difficulty += Load[cur_subject->id];
    }

    if(table->n==1){
        (*(table->argmax_synergy))[0] = *(table->subjects[0]);
        (*(table->argmax_synergy))[1] = *(table->subjects[0]);
    }
    else{
        (*(table->argmax_synergy))[0] = *(table->subjects[0]);
        (*(table->argmax_synergy))[1] = *(table->subjects[1]);
        table->difficulty /= table->n;
        for(int i = 0 ; i < table->n ; i++){
            for(int j = i+1 ; j< table->n ; j++){
                if(Synergy[table->subjects[i]->id][table->subjects[j]->id] >
                   Synergy[(*(table->argmax_synergy))[0].id][(*(table->argmax_synergy))[1].id]){
                    (*(table->argmax_synergy))[0] = *(table->subjects[i]);
                    (*(table->argmax_synergy))[1] = *(table->subjects[j]);
                }
                table->difficulty += Synergy[table->subjects[i]->id][table->subjects[j]->id];
            }
        }
    }
    return SUCCESS;
}

/**
 * add_difficulty_db
 * ï¿???ï¿½ï¿½?ï¿½ï¿½ path?ï¿½ï¿½ difficulty ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ë² ì´?ï¿½ï¿½ï¿?? ì¶”ï¿½???ï¿½ï¿½?ï¿½ï¿½.
 * ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½ï¿??(ID, ï¿?? ?ï¿½ï¿½)?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½ï¿???ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ë¡œï¿½???ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ë°›ëŠ”?ï¿½ï¿½.
 **/
// StatusCode add_difficulty_db(const TimeTable *table) {
//     char dir_path[PATH_LENGTH] = "./dataset/difficulty_calculator";
//     char file_path[PATH_LENGTH + 20];
//     int num_of_data;
//     int total_difficulty;
    
//     FILE *fp = NULL;
//     sprintf(file_path, "%s/num_of_data.dat", dir_path);
//     fp = fopen(file_path, "rb");
//     if(fp == NULL){
//         num_of_data = 0;
//         fp = fopen(file_path, "wb");
//         fwrite(&num_of_data, sizeof(int), 1, fp);
//     }
//     else{
//         fread(&num_of_data, sizeof(int), 1, fp);
//     }
//     fclose(fp);

//     printf("?ï¿½ï¿½?ï¿½ï¿½ ?ï¿½ï¿½ê°„í‘œ?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ï¿?? 0~10?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ï¿?? ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ì£¼ì„¸?ï¿½ï¿½ : ");
//     scanf("%d", &total_difficulty);

//     sprintf(file_path, "%s/data/data%:03d.txt", dir_path, num_of_data);
//     fp = fopen(file_path, "w");

//     fprintf(fp, "%d %d\n", table->n, total_difficulty);
//     for(int i=0;i<table->n;i++){
//         int cur_difficulty;
//         printf("ê³¼ëª© %s?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ï¿?? 0~10?ï¿½ï¿½ ?ï¿½ï¿½?ï¿½ï¿½ï¿?? ?ï¿½ï¿½?ï¿½ï¿½?ï¿½ï¿½ì£¼ì„¸?ï¿½ï¿½ : ", table->subjects[i]->name);
//         scanf("%d", &cur_difficulty);
//         fprintf(fp, "%d %d\n", table->subjects[i]->id, cur_difficulty);
//     }
//     fclose(fp);

//     // sprintf(file_path, "%s/data/data%:03d.txt", dir_path, num_of_data);
//     // fp = fopen(file_path, "w");

//     // int num_of_subject;
//     // scanf("%d %d", &num_of_subject, &total_difficulty);
//     // fprintf(fp, "%d %d\n", num_of_subject, total_difficulty);
//     // for(int i=0;i<num_of_subject;i++){
//     //     int subject, cur_difficulty;
//     //     scanf("%d %d", &subject, &cur_difficulty);
//     //     fprintf(fp, "%d %d\n", subject, cur_difficulty);
//     // }
//     // fclose(fp);

//     num_of_data++;

//     sprintf(file_path, "%s/num_of_data.dat", dir_path);
//     fp = fopen(file_path, "wb");
//     fwrite(&num_of_data, sizeof(int), 1, fp);
//     fclose(fp);
    

//     double Load[MAX_SUBJECT_NUM], Synergy[MAX_SUBJECT_NUM][MAX_SUBJECT_NUM];
//     preprocess_load(&Load);
//     preprocess_synergy(&Synergy);

//     return SUCCESS;
// }