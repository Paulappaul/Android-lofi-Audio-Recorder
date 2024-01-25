if(NOT TARGET oboe::oboe)
add_library(oboe::oboe SHARED IMPORTED)
set_target_properties(oboe::oboe PROPERTIES
    IMPORTED_LOCATION "C:/Users/alcin/.gradle/caches/transforms-3/8799c28099357b0a170005fc9173f5b9/transformed/jetified-oboe-1.8.0/prefab/modules/oboe/libs/android.x86_64/liboboe.so"
    INTERFACE_INCLUDE_DIRECTORIES "C:/Users/alcin/.gradle/caches/transforms-3/8799c28099357b0a170005fc9173f5b9/transformed/jetified-oboe-1.8.0/prefab/modules/oboe/include"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

