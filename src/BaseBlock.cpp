#include <BaseBlock.hpp>

void BaseBlock::toggleAutoIndex()
{
	this->m_autoIndex = true;
}

void BaseBlock::setRoot(const std::string& root)
{

}

void BaseBlock::setClientMaxBodySize(const std::string& size)
{

}

void BaseBlock::insertIndexFiles(const std::set<std::string>& indexes)
{

}

bool BaseBlock::getAutoIndex() const
{
	return this->m_autoIndex;
}

const std::string& BaseBlock::getRoot() const
{
	return this->m_root;
}

std::size_t BaseBlock::getClientMaxBodySize() const
{
	return this->m_clientMaxBodySize;
}

const std::string& BaseBlock::getIndexFile() const
{

}

BaseBlock::BaseBlock():
	m_autoIndex(),
	m_clientMaxBodySize(),
	m_indexes(),
	m_root()
{}

BaseBlock::BaseBlock(BaseBlock& obj):
	m_autoIndex(obj.m_autoIndex),
	m_clientMaxBodySize(obj.m_clientMaxBodySize),
	m_indexes(obj.m_indexes),
	m_root(obj.m_root)
{}