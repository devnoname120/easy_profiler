#include "profiler/profiler.h"
#include "profile_manager.h"
#include <ctime>
#include <chrono>
#include <thread>

using namespace profiler;

#ifdef WIN32
struct ProfPerformanceFrequency {
    LARGE_INTEGER frequency;
    ProfPerformanceFrequency() { QueryPerformanceFrequency(&frequency); }
} const WINDOWS_CPU_INFO;
#endif

inline timestamp_t getCurrentTime()
{
#ifdef WIN32
	//see https://msdn.microsoft.com/library/windows/desktop/dn553408(v=vs.85).aspx
	LARGE_INTEGER elapsedMicroseconds;
	if (!QueryPerformanceCounter(&elapsedMicroseconds))
		return 0;
	elapsedMicroseconds.QuadPart *= 1000000000LL;
    elapsedMicroseconds.QuadPart /= WINDOWS_CPU_INFO.frequency.QuadPart;
	return (timestamp_t)elapsedMicroseconds.QuadPart;
#else
	std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_point;
	time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
	return time_point.time_since_epoch().count();
#endif
}

BaseBlockData::BaseBlockData(timestamp_t _begin_time, block_id_t _descriptor_id)
    : m_begin(_begin_time)
    , m_end(0)
    , m_id(_descriptor_id)
{

}

Block::Block(const char*, block_type_t _block_type, block_id_t _descriptor_id, const char* _name)
    : BaseBlockData(getCurrentTime(), _descriptor_id)
    , m_name(_name)
{
    if (_block_type != BLOCK_TYPE_BLOCK)
    {
        m_end = m_begin;
    }
}

void Block::finish()
{
    m_end = getCurrentTime();
}

Block::~Block()
{
    if (!isFinished())
        ::profiler::endBlock();
}
