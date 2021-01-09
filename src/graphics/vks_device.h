#include "vks_window.h"

namespace vks
{

    class VksDevice
    {
    public:
        VksDevice(VksWindow &window);
        ~VksDevice();
        void createInstance();

    private:
        VkInstance instance;
        VksWindow &window;

    };

}

