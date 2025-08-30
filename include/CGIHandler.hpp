#include <CommonUtils.hpp>

class CGIHandler
{
	private:
		int m_pipe[2];
	public:
		CGIHandler();
		~CGIHandler();
};