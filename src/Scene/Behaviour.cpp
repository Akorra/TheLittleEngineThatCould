#include "TLETC/Scene/Behaviour.h"
#include "TLETC/Scene/Entity.h"

namespace TLETC {

Behaviour::Behaviour() : entity_(nullptr), enabled_(true)
{
}

Behaviour::~Behaviour() 
{
}

Input* Behaviour::GetInput() const 
{
    if (entity_) {
        return entity_->GetInput();
    }
    return nullptr;
}

} // namespace TLETC
