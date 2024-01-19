if(NOT TARGET oboe::oboe)
add_library(oboe::oboe SHARED IMPORTED)
set_target_properties(oboe::oboe PROPERTIES
    IMPORTED_LOCATION "C:/Users/alcin/.gradle/caches/transforms-3/993aeec82d69b298a729f25ad63e1a15/transformed/oboe-1.8.0/prefab/modules/oboe/libs/android.x86_64/liboboe.so"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/alcin/.gradle/caches/transforms-3/993aeec82d69b298a729f25ad63e1a15/transformed/oboe-1.8.0/prefab/modules/oboe/include"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

