#ifndef GDEXTENSION_REGISTRATION_HPP_
#define GDEXTENSION_REGISTRATION_HPP_

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_libgdmaze(ModuleInitializationLevel p_level);
void uninitialize_libgdmaze(ModuleInitializationLevel p_level);

#endif

