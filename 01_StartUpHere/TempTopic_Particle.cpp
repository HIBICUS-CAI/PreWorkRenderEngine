#include "TempTopic_Particle.h"
#include "RSRoot_DX11.h"

RSPass_PriticleSimulate::RSPass_PriticleSimulate(
    std::string& _name, PASS_TYPE _type, RSRoot_DX11* _root) :
    RSPass_Base(_name, _type, _root)
{

}

RSPass_PriticleSimulate::RSPass_PriticleSimulate(
    const RSPass_PriticleSimulate& _source) :
    RSPass_Base(_source)
{

}

RSPass_PriticleSimulate::~RSPass_PriticleSimulate()
{

}

RSPass_PriticleSimulate* RSPass_PriticleSimulate::ClonePass()
{
    return new RSPass_PriticleSimulate(*this);
}

bool RSPass_PriticleSimulate::InitPass()
{
    // TEMP-------------
    return false;
}

void RSPass_PriticleSimulate::ReleasePass()
{

}

void RSPass_PriticleSimulate::ExecuatePass()
{

}

bool RSPass_PriticleSimulate::CreateShaders()
{
    // TEMP-----------
    return false;
}

bool RSPass_PriticleSimulate::CreateBuffers()
{
    // TEMP---------------
    return false;
}

bool RSPass_PriticleSimulate::CreateViews()
{
    // TEMP-----------------
    return false;
}
