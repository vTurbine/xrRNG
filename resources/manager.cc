#include "stdafx.h"

#include "resources/manager.h"

#include <xrEngine/WaveForm.h>


ResourceManager resources;

enum ShaderBaseChunkType
{
    eConstant,
    eMatrix,
    eBlender
};

struct BlenderDescription
{
    CLASS_ID    class_id;
    string128   name;
    string32    computer_name;
    u32         time;
    u16         version;
};


/**
 *
 */
void
ResourceManager::OnDeviceCreate
        ( std::string const     &file_name
        )
{
    std::string const signature { "shENGINE" };
    string32 id;
    IReader *rstream = FS.r_open(file_name.c_str());
    
    R_ASSERT2(rstream, file_name);
    
    rstream->r(&id, signature.size());
    if (strncmp(id, signature.c_str(), signature.size()) == 0)
    {
        FATAL("Unsupported blender library. Compressed?");
    }
    
    // Load constants
    auto fs = rstream->open_chunk(eConstant);
    if (fs)
    {
        string256 name;

        while (!fs->eof())
        {
            fs->r_stringZ(name, sizeof(name));
            fs->advance(4 * 4); // r,g,b,a
            Msg("! Constant %s skipped..", name);
        }
        fs->close();
    }

    // Load matrices
    fs = rstream->open_chunk(eMatrix);
    if (fs)
    {
        string256 name;

        while (!fs->eof())
        {
            fs->r_stringZ(name, sizeof(name));
            fs->advance(2 * 4 + 5 * sizeof(WaveForm)); // mode, tcm, scale, rotate, scroll
            Msg("! Matrix %s skipped..", name);
        }
        fs->close();
    }

    // Load blenders
    fs = rstream->open_chunk(eBlender);
    if (fs)
    {
        IReader *chunk;
        int chunk_id = 0;

        while ((chunk = fs->open_chunk(chunk_id)))
        {
            BlenderDescription desc;
            chunk->r(&desc, sizeof (desc));

            //auto b = CreateBlender(desc.class_id);
            
            if (0)
            {
#if 0
                if (b->GetDescription().version != desc.version)
                {
                    Msg("! Version conflict in blender '%s'", desc.name);
                }

                chunk->seek(0);
                b->Load(*chunk, desc.version);

                std::string name{ desc.name };
                auto &iterator = blenders_.insert(std::make_pair(name, b));
                R_ASSERT2(iterator.second, "shader.xr - found duplicate name");
#endif
            }
            else
            {
                Msg("! Renderer doesn't support blender '%s'", desc.name);
            }

            chunk->close();
            chunk_id++;
        }
        fs->close();
    }

    FS.r_close(rstream);
}


/**
 *
 */
void
ResourceManager::OnDeviceDestroy()
{
}


#if 0
/**
 *
 */
void
ResourceManager::ParseList
        ( const std::string &list
        , std::vector<std::string> &strings
        )
{
    strings.clear();

    if (list.size() == 0)
    {
        strings.push_back("$null");
        return;
    }

    // Split string
    std::function<void ( const std::string&
                       , std::vector<std::string>&
                       , const char
    )> split_with;

    split_with =
        [&]( const std::string &list
           , std::vector<std::string> &strings
           , const char delimiter
           )
    {
        if (list.size() == 0)
        {
            return;
        }

        auto &iterator = std::find(list.cbegin(), list.cend(), delimiter);
        strings.push_back({ list.cbegin(), iterator });

        if (iterator != list.cend())
        {
            split_with({ ++iterator, list.cend() }, strings, delimiter);
        }
    };

    split_with(list, strings, ',');

    // Remove extensions

}

/*!
 * Texture types supported by the engine
 */
const std::vector<std::string> texture_extensions =
{
    "tga",
    "dds",
    "bmp",
    "ogm"
};

/*!
 * \brief   Removes image format extensions from a given file name list
 */
void
ResourceManager::RemoveTexturesExtension
        ( std::vector<std::string> &strings
        )
{
    auto remove_extension = [](const std::string &file_name) -> std::string
    {
        for (const auto &extension : texture_extensions)
        {
            auto &iterator = std::find_end( file_name.cbegin()
                                          , file_name.cend()
                                          , extension.cbegin()
                                          , extension.cend()
            );

            if (iterator != file_name.cend())
            {
                return { file_name.cbegin(), --iterator };
            }
        }

        return file_name;
    };

    std::transform( strings.cbegin()
                  , strings.cend()
                  , strings.begin()
                  , remove_extension
    );
}
#endif
