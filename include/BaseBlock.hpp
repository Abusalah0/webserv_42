#include <string>
#include <set>

class BaseBlock
{
	private:
		bool m_autoIndex;
		std::string m_root;
		std::size_t m_clientMaxBodySize;
		std::set<std::string> m_indexes;
	public:
		void toggleAutoIndex();
		void setRoot(const std::string& root);
		void setClientMaxBodySize(const std::string& size);
		void insertIndexFiles(const std::set<std::string>& indexes);
		bool getAutoIndex() const;
		const std::string& getRoot() const;
		std::size_t getClientMaxBodySize() const;
		const std::string& getIndexFile() const;
		BaseBlock();
		BaseBlock(BaseBlock& obj);
		virtual ~BaseBlock();
};