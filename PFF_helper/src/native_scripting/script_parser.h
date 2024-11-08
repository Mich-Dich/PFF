#pragma once

#include "script_scanner.h"

namespace PFF {

	struct PFF_variable {
		std::string						type = {};
		std::string						identifier = {};
		std::string						specifiers = {};
	};

	struct PFF_class {
		std::string						class_name = {};
		std::string						class_name_without_namespace = {};
		std::string						parent_class_name = {};
		std::string						specifiers = {};
		std::filesystem::path			full_filepath = {};
		std::list<PFF_variable>			variables = {};
	};

	struct PFF_struct {
		std::string						struct_name = {};
		std::string						struct_name_without_namespace = {};
		std::string						specifiers = {};
		std::filesystem::path			full_filepath = {};
		std::list<PFF_variable>			variables = {};
	};

	class script_parser {
	public:

		script_parser(std::vector<token>& tokens, const std::filesystem::path& full_filepath)
			: m_Tokens(tokens), m_full_filepath(full_filepath) {}

		std::string generate_header_file();
		void debug_print();
		void parse();

		PFF_DEFAULT_GETTER(std::vector<PFF_class>&, classes)
		static std::string get_filename_as_class_name(std::string filename);

		FORCEINLINE bool can_generate_header_file() const { return m_structs.size() != 0 || m_classes.size() != 0; }

	private:

		struct sorting_category {

			std::string						category_name{};
			std::vector<PFF_variable>		variables{};
		};

		void sort_by_categories(std::vector<sorting_category>& sorted, const std::list<PFF_variable>& variables);
		std::string generate_display_function(const std::string& name, const std::string& name_without_namespace, const std::string& specifiers, const std::filesystem::path& full_filepath, const std::list<PFF_variable>& variables, bool is_class);
		std::string generate_serialization_function(const std::string& name, const std::string& name_without_namespace, const std::string& specifiers, const std::filesystem::path& full_filepath, const std::list<PFF_variable>& variables);

		std::string parse_specifiers_value(const std::string& specifiers, const std::string& key, std::string default_value = "0");
		void parse_class(const std::string& specifiers);
		void parse_struct(const std::string& specifiers);
		PFF_variable parse_variable();
		void parse_property_specifiers(std::string& specifiers);

		FORCEINLINE bool is_var_pff_struct_or_class(const std::string& var_type);
		FORCEINLINE const token& expect(token_type type);
		FORCEINLINE bool match(token_type type);
		FORCEINLINE void advance() { m_current_token++; m_Current_iter++; }
		FORCEINLINE void advance_to_token(const token_type type);

		std::vector<token>::iterator	m_Current_iter;
		u64								m_current_token;
		std::filesystem::path			m_full_filepath;

		std::vector<token>&				m_Tokens;
		std::vector<PFF_class>			m_classes;
		std::vector<PFF_struct>			m_structs;
		std::string						m_current_namespace;
	};

}
