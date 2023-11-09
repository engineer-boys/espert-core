#ifndef ESPERT_CORE_RESOURCES_RESOURCETYPES_HH
#define ESPERT_CORE_RESOURCES_RESOURCETYPES_HH

#include "esppch.hh"

#include "Core/Resources/ResourceUtils.hh"

namespace esp
{
  class Resource
  {
   public:
    Resource(const fs::path& path, uint64_t data_size, std::unique_ptr<void, VOID_DELETER_TYPE> data) :
        m_path(path), m_data_size(data_size), m_data(std::move(data))
    {
    }

    virtual ~Resource() {}

    PREVENT_COPY(Resource);

    inline const fs::path& get_path() const { return m_path; }
    inline const std::string get_filename() const { return m_path.filename(); }
    inline const void* get_data() { return m_data.get(); }

   private:
    fs::path m_path;
    uint64_t m_data_size;
    std::unique_ptr<void, VOID_DELETER_TYPE> m_data;
  };

  struct ResourceParams
  {
  };

  class BinaryResource : public Resource
  {
   public:
    BinaryResource(const fs::path& path, uint64_t data_size, std::unique_ptr<void, VOID_DELETER_TYPE> data) :
        Resource(path, data_size, std::move(data))
    {
    }

    PREVENT_COPY(BinaryResource);
  };

  struct BinaryResourceParams : public ResourceParams
  {
  };

  class TextResource : public Resource
  {
   public:
    TextResource(const fs::path& path,
                 uint64_t data_size,
                 std::unique_ptr<void, VOID_DELETER_TYPE> data,
                 uint64_t num_of_lines) :
        Resource(path, data_size, std::move(data)),
        m_num_of_lines(num_of_lines)
    {
    }

    PREVENT_COPY(TextResource);

    inline const uint64_t& get_num_of_lines() { return m_num_of_lines; }

   private:
    uint64_t m_num_of_lines;
  };

  struct TextResourceParams : public ResourceParams
  {
  };

} // namespace esp

#endif // ESPERT_CORE_RESOURCES_RESOURCETYPES_HH