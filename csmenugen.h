#include <fstream>
#include <map>
#include <string>
#include <initializer_list>
#include <functional>
#include <vector>
#include <memory>

template <class KeyMap>
class Menu
{
    std::shared_ptr<std::function<void(std::string, std::string)>> compile;
public:
    Menu() = delete;
    Menu(const Menu<KeyMap>&) = delete;
    Menu(Menu<KeyMap>&&) = delete;


    Menu(std::string action) :
    compile{std::make_shared<std::function<void(std::string, std::string)>>([action](std::string root, std::string name)
    {
        std::ofstream{name + ".cfg"} << action << std::endl << "exec " << root << ".cfg";
    })}
    { }

    Menu(const std::initializer_list<std::pair<int, const Menu<KeyMap>&>>& submenu) :
    compile{std::make_shared<std::function<void(std::string, std::string)>>(
        [
            submenu{[&submenu]()
            {
                std::map<int, std::shared_ptr<std::function<void(std::string, std::string)>>> result;
                for(auto i = submenu.begin(); i != submenu.end(); i++)
                    result[i->first] = i->second.compile;
                return result;
            }()}
        ](std::string root, std::string name)
        {
            std::ofstream ofstr{name + ".cfg"};

            for(int i = 0; i < KeyMap().size(); i++)
                if(submenu.count(i) > 0)
                    ofstr << "bind " << KeyMap()[i] << " \"exec " << name + std::to_string(i) << ".cfg\"" << std::endl;
                else
                    ofstr << "unbind " << KeyMap()[i] << std::endl;

            for(auto i = submenu.begin(); i != submenu.end(); i++)
                (*i->second)(root, name + std::to_string(i->first));
        })}
    { }
    
    void Compile(std::string root, std::string name) const { (*compile)(root, name); }
};