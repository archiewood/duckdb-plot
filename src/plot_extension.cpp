#define DUCKDB_EXTENSION_MAIN

#include "plot_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>


namespace duckdb {

inline void PlotScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Plot "+name.GetString()+" 🐥");;
        });
}

// Function to left pad a string
std::string LeftPad(const std::string &str, size_t width) {
    if (str.length() >= width) {
        return str;
    }
    return std::string(width - str.length(), ' ') + str;
}

std::string repeat_string(const std::string &str, int times) {
    std::string result;
    for (int i = 0; i < times; ++i) {
        result += str;
    }
    return result;
}

inline void BarScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &value_vector = args.data[0];
    auto &label_vector = args.data[1];
    static const char *Block = "\xE2\x96\x88";
    BinaryExecutor::Execute<int32_t, string_t, string_t>(
        value_vector, label_vector, result, args.size(),
        [&](int32_t value, string_t label) {
            string padded_label = LeftPad(label.GetString(), 22); // adjust width as needed
            std::string bar_string = repeat_string(Block, value);
            return StringVector::AddString(result, padded_label + " │" + bar_string + " " + std::to_string(value));
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto plot_scalar_function = ScalarFunction("plot", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PlotScalarFun);
    ExtensionUtil::RegisterFunction(instance, plot_scalar_function);

    // Register the Bar function
    auto bar_scalar_function = ScalarFunction("plot_bar", {LogicalType::INTEGER, LogicalType::VARCHAR}, LogicalType::VARCHAR, BarScalarFun);
    ExtensionUtil::RegisterFunction(instance, bar_scalar_function);
}

void PlotExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string PlotExtension::Name() {
	return "plot";
}

std::string PlotExtension::Version() const {
#ifdef EXT_VERSION_PLOT
	return EXT_VERSION_PLOT;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void plot_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::PlotExtension>();
}

DUCKDB_EXTENSION_API const char *plot_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
