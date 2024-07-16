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

inline void BarScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &input_vector = args.data[0];
    UnaryExecutor::Execute<int32_t, string_t>(
        input_vector, result, args.size(),
        [&](int32_t input) {
            string bar_string(input, '*');
            return StringVector::AddString(result, bar_string);
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto plot_scalar_function = ScalarFunction("plot", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PlotScalarFun);
    ExtensionUtil::RegisterFunction(instance, plot_scalar_function);

    // Register the Bar function
    auto bar_scalar_function = ScalarFunction("plot_bar", {LogicalType::INTEGER}, LogicalType::VARCHAR, BarScalarFun);
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
