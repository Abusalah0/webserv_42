#include <CommonUtils.hpp>

class CGIHandler
{
	private:
		int m_pipe[2];
		pid_t m_pid;
	public:
		CGIHandler();
		~CGIHandler();
};