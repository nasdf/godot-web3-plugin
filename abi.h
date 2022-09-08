#ifndef ABI_H
#define ABI_H

#include "core/io/resource_loader.h"
#include "core/resource.h"

class ABI : public Resource {
  GDCLASS(ABI, Resource);

  // struct Function {
  //   String type;
  //   String name;
  //   String state_mutability;

  //   // inputs
  //   // outputs
  // };

protected:
  static void _bind_methods();

private:
  Array entries;
  // List<Function> functions;
  // Vector<ABI::Event> events;
  // Vector<ABI::Error> errors;

public:
  Error from_json(const String &p_json);

  void set_entries(const Array &p_entries);
  Array get_entries();
};

class ResourceFormatLoaderABI : public ResourceFormatLoader {
  GDCLASS(ResourceFormatLoaderABI, ResourceFormatLoader);
public:
  virtual RES load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_no_subresource_cache = false);
  virtual void get_recognized_extensions(List<String> *r_extensions) const;
  virtual bool handles_type(const String &p_type) const;
  virtual String get_resource_type(const String &p_path) const;
};

#endif // ABI_H