#pragma once
#include "AnimationBase.hpp"
#include "UI/DSA/Node.hpp" 

namespace UI::Animations {

    class ScaleAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode; 

        float startScale;       
        float endScale;         
        float totalDuration;    
        float elapsedTime;      

    public:
        ScaleAnimation(UI::DSA::Node* node, float start, float end, float duration);
        void update(float dt) override;
        bool isFinished() const override;
    };

} // namespace UI::Animations