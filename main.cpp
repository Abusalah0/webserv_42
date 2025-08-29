#include <sys/types.h>
#include <string>
#include <exception>

struct HTTPStatus
{
	ushort code;
	const char* msg;
};

static const HTTPStatus statusTable[] = {
	{404, "Not Found"},
	{403, "Forbidden"},
	{505, "HTTP Version Not Supported"},
	{0, 0}
};

class HTTPException: public std::exception
{
	private:
		ushort m_code;
		std::string m_msg;
	public:
		HTTPException(ushort code);
		virtual ~HTTPException() throw();
		ushort getErrorCode();
		const char* what() const throw();
};

static const char* statusTableLookup(ushort code)
{
    for (int i = 0; statusTable[i].code != 0; ++i)
	{
        if (statusTable[i].code == code)
            return statusTable[i].msg;
    }
    return "Unknown Error";
}

HTTPException::HTTPException(ushort code):
	m_code(code)
{
	const char* msg = statusTableLookup(code);
	int i = 0;
	char digits[3];
	while (code)
	{
		digits[i] = code % 10 + 0x30;
		code /= 10;
		++i;
	}
	--i;
	while (i > -1)
	{
		this->m_msg.push_back(digits[i]);
		--i;
	}
	this->m_msg.push_back(' ');
	this->m_msg.append(msg);
}

HTTPException::~HTTPException() throw()
{}

ushort HTTPException::getErrorCode()
{
	return this->m_code;
}

const char* HTTPException::what() const throw()
{
	return this->m_msg.c_str();
}

int main()
{
	throw HTTPException(403);
}