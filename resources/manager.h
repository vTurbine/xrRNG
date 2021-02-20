#ifndef RESOURCES_MANAGER_H_
#define RESOURCES_MANAGER_H_

#include <string>


class ResourceManager
{
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void OnDeviceCreate(std::string const &file_name);
    void OnDeviceDestroy();


};

extern ResourceManager resources;

#endif // RESOURCES_MANAGER_H_
