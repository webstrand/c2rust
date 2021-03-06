#ifndef CROSSCHECK_PLUGIN_CONFIG_H
#define CROSSCHECK_PLUGIN_CONFIG_H

#include "llvm/Support/YAMLTraits.h"

#include <string>
#include <variant>
#include <vector>

struct XCheck {
    enum Tag {
        UNKNOWN = 0,
        FUNCTION_ENTRY = 1,
        FUNCTION_EXIT = 2,
        FUNCTION_ARG = 3,
        FUNCTION_RETURN = 4,
    };

    enum Type {
        DEFAULT,
        DISABLED,
        FIXED,
        DJB2,
        CUSTOM,
    } type;
    std::variant<std::monostate, uint64_t, std::string> data;

    XCheck(Type ty = DEFAULT) : type(ty), data() {}
    XCheck(Type ty, const std::string &s) : type(ty), data(s) {}

    // Required by IO::mapOptional
    bool operator==(const XCheck &other) const {
        return type == other.type && data == other.data;
    }
};

LLVM_YAML_DECLARE_MAPPING_TRAITS(XCheck)
LLVM_YAML_DECLARE_ENUM_TRAITS(XCheck::Tag)
LLVM_YAML_IS_STRING_MAP(XCheck)

namespace llvm {
namespace yaml {
template<>
inline void yamlize(IO &io, XCheck &xc, bool, EmptyContext &ctx) {
    if (io.outputting()) {
        // TODO: implement
        llvm_unreachable("Unimplemented");
    } else {
        io.beginEnumScalar();
#define CHECK_ENUM(_s, _e)               \
    if (io.matchEnumScalar(_s, false)) { \
        xc.type = XCheck::_e;            \
        return;                          \
    }
        CHECK_ENUM("default",  DEFAULT);
        CHECK_ENUM("none",     DISABLED);
        CHECK_ENUM("disabled", DISABLED);
#undef CHECK_ENUM
        // Skip calling io.endEnumScalar(),
        // since that just exits with an error
        // for all values not covered above

        io.beginMapping();
#define CHECK_DATA(_s, _dt, _e) do {              \
        EmptyContext ctx;                         \
        Optional<_dt> data;                       \
        io.mapOptionalWithContext(_s, data, ctx); \
        if (data) {                               \
            xc.type = XCheck::_e;                 \
            xc.data = *data;                      \
            return;                               \
        }                                         \
    } while (0)
        CHECK_DATA("fixed",  uint64_t,    FIXED);
        CHECK_DATA("djb2",   std::string, DJB2);
        CHECK_DATA("custom", std::string, CUSTOM);
#undef CHECK_DATA
        io.endMapping();

        // We couldn't handle this input, so throw an error
        io.setError("Unknown cross-check type");
    }
}
} // namespace yaml
} // namespace llvm

struct ExtraXCheck {
    XCheck::Tag tag;
    std::string custom;
};

LLVM_YAML_DECLARE_MAPPING_TRAITS(ExtraXCheck)
LLVM_YAML_IS_SEQUENCE_VECTOR(ExtraXCheck)

struct DefaultsConfig {
    llvm::Optional<bool> disable_xchecks;
    llvm::Optional<XCheck> entry;
    llvm::Optional<XCheck> exit;
    llvm::Optional<XCheck> all_args;
    llvm::Optional<XCheck> ret;
    // TODO: do we want entry/exit_extra here???

    DefaultsConfig() = default;
    DefaultsConfig(llvm::yaml::IO &io) {
        io.mapOptional("disable_xchecks", disable_xchecks);
        io.mapOptional("entry",     entry);
        io.mapOptional("exit",      exit);
        io.mapOptional("all_args",  all_args);
        io.mapOptional("return",    ret);
    }

    // Update the optionals in this config with the contents
    // of another DefaultsConfig
    void update(const DefaultsConfig &other) {
#define UPDATE_FIELD(field) if (other.field) { this->field = *other.field; }
        UPDATE_FIELD(disable_xchecks);
        UPDATE_FIELD(entry);
        UPDATE_FIELD(exit);
        UPDATE_FIELD(all_args);
        UPDATE_FIELD(ret);
#undef UPDATE_FIELD
    }
};

struct FunctionConfig {
    std::string name;
    llvm::Optional<bool> disable_xchecks;
    llvm::Optional<XCheck> entry;
    llvm::Optional<XCheck> exit;
    llvm::Optional<XCheck> all_args;
    std::map<std::string, XCheck> args;
    llvm::Optional<XCheck> ret;
    llvm::Optional<std::string> ahasher;
    llvm::Optional<std::string> shasher;
    // TODO: nested
    std::vector<ExtraXCheck> entry_extra;
    std::vector<ExtraXCheck> exit_extra;

    FunctionConfig(std::string_view n) : name(n) {}

    void yamlize(llvm::yaml::IO &io) {
        io.mapOptional("disable_xchecks", disable_xchecks);
        io.mapOptional("entry",     entry);
        io.mapOptional("exit",      exit);
        io.mapOptional("all_args",  all_args);
        io.mapOptional("args",      args);
        io.mapOptional("return",    ret);
        io.mapOptional("ahasher",   ahasher);
        io.mapOptional("shasher",   shasher);
        io.mapOptional("entry_extra", entry_extra);
        io.mapOptional("exit_extra",  exit_extra);
    }

    void update(const FunctionConfig &other) {
        assert(this->name == other.name && "Mismatched names in FunctionConfig::update()");
#define UPDATE_FIELD(field) if (other.field) { this->field = *other.field; }
        UPDATE_FIELD(disable_xchecks);
        UPDATE_FIELD(entry);
        UPDATE_FIELD(exit);
        UPDATE_FIELD(all_args);
        UPDATE_FIELD(ret);
        UPDATE_FIELD(ahasher);
        UPDATE_FIELD(shasher);
#undef UPDATE_FIELD
        for (auto &it : other.args)
            args.insert_or_assign(it.first, it.second);
        // Append other.entry_extra to ours
        // FIXME: should replace the existing entries instead???
        entry_extra.insert(entry_extra.end(),
                           other.entry_extra.begin(),
                           other.entry_extra.end());
        exit_extra.insert(exit_extra.end(),
                          other.exit_extra.begin(),
                          other.exit_extra.end());
    }
};

struct StructConfig {
    std::string name;
    llvm::Optional<bool> disable_xchecks;
    llvm::Optional<std::string> field_hasher;
    llvm::Optional<std::string> custom_hash;
    std::map<std::string, XCheck> fields;
    llvm::Optional<std::string> ahasher;
    llvm::Optional<std::string> shasher;

    StructConfig(std::string_view n) : name(n) {}

    void yamlize(llvm::yaml::IO &io) {
        io.mapOptional("disable_xchecks", disable_xchecks);
        io.mapOptional("field_hasher",  field_hasher);
        io.mapOptional("custom_hash",   custom_hash);
        io.mapOptional("fields",        fields);
        io.mapOptional("ahasher",       ahasher);
        io.mapOptional("shasher",       shasher);
    }

    void update(const StructConfig &other) {
        assert(this->name == other.name && "Mismatched names in StructConfig::update()");
#define UPDATE_FIELD(field) if (other.field) { this->field = *other.field; }
        UPDATE_FIELD(disable_xchecks);
        UPDATE_FIELD(field_hasher);
        UPDATE_FIELD(custom_hash);
        UPDATE_FIELD(ahasher);
        UPDATE_FIELD(shasher);
#undef UPDATE_FIELD
        for (auto &it : other.fields)
            fields.insert_or_assign(it.first, it.second);
    }
};

typedef std::variant<std::monostate, DefaultsConfig,
        FunctionConfig, StructConfig> ItemConfig;
typedef std::vector<ItemConfig> FileConfig;
typedef std::map<std::string, FileConfig> Config;

LLVM_YAML_DECLARE_MAPPING_TRAITS(FunctionConfig)
LLVM_YAML_DECLARE_MAPPING_TRAITS(StructConfig)
LLVM_YAML_DECLARE_MAPPING_TRAITS(ItemConfig)
LLVM_YAML_IS_SEQUENCE_VECTOR(ItemConfig)
LLVM_YAML_IS_STRING_MAP(FileConfig)

#endif // CROSSCHECK_PLUGIN_CONFIG_H
