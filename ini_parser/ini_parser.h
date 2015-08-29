#ifndef QIHOO_INI_PARSER_H_
#define QIHOO_INI_PARSER_H_

#include <string>
#include <map>

namespace qh
{
    class INIParser
    {
    public:
        INIParser();
        // Change to virtual because destory this class may be inherit by other class, it's a good habit
        virtual ~INIParser();

        //! \brief ����һ�������ϵ�INI�ļ�
        //! \param[in] - const std::string & ini_file_path
        //! \return - bool
        bool Parse(const std::string& ini_file_path);

        //! \brief ����һ������INI��ʽ���ڴ����ݡ�
        //!   ���磺ini_data="a:1||b:2||c:3"
        //!         ����<code>Parse(ini_data, ini_data_len, "||", ":")</code>���ɽ�����������ݡ�
        //!         �������֮�� 
        //!         Get("a")=="1" && Get("b")=="2" && Get("c")=="3"
        //! \param[in] - const char * ini_data
        //! \param[in] - size_t ini_data
        //! \param[in] - const std::string & line_seperator
        //! \param[in] - const std::string & key_value_seperator
        //! \return - bool
        bool Parse(const char* ini_data, size_t ini_data_len, const std::string& line_seperator = "\n", const std::string& key_value_seperator = "=");

        //! \brief ��Ĭ��section�в���ĳ��key���������ҵ���value������Ҳ���������һ���մ�
        //! \param[in] - const std::string & key
        //! \param[in] - bool * found - ���������true�����ҵ����key
        //! \return - const std::string& - ���صľ���key��Ӧ��value
        const std::string& Get(const std::string& key, bool* found);

        const std::string& Get(const std::string& section, const std::string& key, bool* found);

    private:
        // Helper functions
        bool ParseLines(std::string& lines, const std::string& line_seperator, const std::string& key_value_seperator);

        void TrimLine(std::string& line, const std::string& symbol, bool all = true) const;

        void TrimLineComment(std::string& line, const std::string& comment_symbol) const;

        bool ParseProperty(std::string& property, const std::string& key_value_seperator);

        std::string GetSectionName(std::string& section_line, const std::string& begin_symbol, const std::string& end_symbol);
        

        // Symbols of comment and section
        const std::string comment_symbol;
        
        const std::string section_start_symbol;

        const std::string section_end_symbol;
       
        const std::string default_retVal;

        // Fields
        std::string curr_active_section;
        
        std::map<std::string, std::string> global_key_value_map;

        std::map<std::string, std::map<std::string, std::string> > section_key_value_map;
    };
}

#endif

