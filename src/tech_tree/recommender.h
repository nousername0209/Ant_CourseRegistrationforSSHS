//
// Created by 김주환 on 25. 11. 9.
//

#ifndef TECH_RECOMMENDER_H
#define TECH_RECOMMENDER_H

#include "../struct.h"

StatusCode get_z_score(const Subject *subject, double raw_score, SubjectZScore *z_array, int z_count);

StatusCode recommend_techtree(const SubjectZScore *z_array, int z_count, const TechTree *trees, int tree_count, TechTree *recommended_tree);

#endif //TECH_RECOMMENDER_H
