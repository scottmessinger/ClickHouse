#include <Interpreters/AsynchronousInsertLog.h>

#include <base/getFQDNOrHostName.h>
#include <DataTypes/DataTypeDate.h>
#include <DataTypes/DataTypeDateTime.h>
#include <DataTypes/DataTypeDateTime64.h>
#include <DataTypes/DataTypeLowCardinality.h>
#include <DataTypes/DataTypeString.h>
#include <DataTypes/DataTypesNumber.h>
#include <DataTypes/DataTypeEnum.h>
#include <Parsers/ASTInsertQuery.h>
#include <Parsers/queryToString.h>


namespace DB
{

ColumnsDescription AsynchronousInsertLogElement::getColumnsDescription()
{
    auto type_status = std::make_shared<DataTypeEnum8>(
        DataTypeEnum8::Values
        {
            {"Ok",           static_cast<Int8>(Status::Ok)},
            {"ParsingError", static_cast<Int8>(Status::ParsingError)},
            {"FlushError",   static_cast<Int8>(Status::FlushError)},
        });

    auto type_data_kind = std::make_shared<DataTypeEnum8>(
        DataTypeEnum8::Values
        {
            {"Parsed",       static_cast<Int8>(DataKind::Parsed)},
            {"Preprocessed", static_cast<Int8>(DataKind::Preprocessed)},
        });

    return ColumnsDescription{
        {"hostname", std::make_shared<DataTypeLowCardinality>(std::make_shared<DataTypeString>())},
        {"event_date", std::make_shared<DataTypeDate>()},
        {"event_time", std::make_shared<DataTypeDateTime>()},
        {"event_time_microseconds", std::make_shared<DataTypeDateTime64>(6)},

        {"query", std::make_shared<DataTypeString>()},
        {"database", std::make_shared<DataTypeLowCardinality>(std::make_shared<DataTypeString>())},
        {"table", std::make_shared<DataTypeLowCardinality>(std::make_shared<DataTypeString>())},
        {"format", std::make_shared<DataTypeLowCardinality>(std::make_shared<DataTypeString>())},
        {"query_id", std::make_shared<DataTypeString>()},
        {"bytes", std::make_shared<DataTypeUInt64>()},
        {"rows", std::make_shared<DataTypeUInt64>()},
        {"exception", std::make_shared<DataTypeString>()},
        {"status", type_status},
        {"data_kind", type_data_kind},

        {"flush_time", std::make_shared<DataTypeDateTime>()},
        {"flush_time_microseconds", std::make_shared<DataTypeDateTime64>(6)},
        {"flush_query_id", std::make_shared<DataTypeString>()},
        {"timeout_milliseconds", std::make_shared<DataTypeUInt64>()},
    };
}

void AsynchronousInsertLogElement::appendToBlock(MutableColumns & columns) const
{
    size_t i = 0;

    columns[i++]->insert(getFQDNOrHostName());
    auto event_date = DateLUT::instance().toDayNum(event_time).toUnderType();
    columns[i++]->insert(event_date);
    columns[i++]->insert(event_time);
    columns[i++]->insert(event_time_microseconds);

    columns[i++]->insert(query_for_logging);
    columns[i++]->insert(database);
    columns[i++]->insert(table);
    columns[i++]->insert(format);
    columns[i++]->insert(query_id);
    columns[i++]->insert(bytes);
    columns[i++]->insert(rows);
    columns[i++]->insert(exception);
    columns[i++]->insert(status);
    columns[i++]->insert(data_kind);

    columns[i++]->insert(flush_time);
    columns[i++]->insert(flush_time_microseconds);
    columns[i++]->insert(flush_query_id);
    columns[i++]->insert(timeout_milliseconds);
}

}
