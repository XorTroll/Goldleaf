#pragma once

#include <memory>
#include <string>
#include <vector>

namespace tin::ui
{
    class IMode
    {
        protected:
            IMode(std::string name);

        public:
            const std::string m_name;

            virtual void OnSelected() = 0;
    };

    class Category final
    {
        private:
            std::vector<std::unique_ptr<IMode>> m_modes;
            IMode* m_currentMode;

        public:
            const std::string m_name;

            Category(std::string name);

            void AddMode(std::unique_ptr<IMode> mode);
            void OnSelected();
    };
}