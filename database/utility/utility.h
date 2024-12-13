#ifndef MY_UTILITY__H_GUARD
#define MY_UTILITY__H_GUARD

#include <vector>
#include "ptr_box.h"
#include "string_view.h"
#include "../data_struct/data_struct_fwd.h"

data_struct::DynamicArray<SView> sv_split (SView const&, char);
data_struct::DynamicArray<SView> sv_split_into_words (SView const&);

std::string join_views (data_struct::DynamicArray<SView> const&, std::string const&);

std::vector<std::string> split (std::string const&, char);

std::string current_time_str();

#endif