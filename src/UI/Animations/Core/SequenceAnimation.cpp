#include "UI/Animations/Core/SequenceAnimation.hpp"

namespace UI::Animations {

    void SequenceAnimation::add(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            sequence.push(std::move(anim));
        }
    }

    void SequenceAnimation::update(float dt) {
        // Thay vì dùng 'if', ta dùng 'while' để tận dụng tối đa lượng dt
        while (!sequence.empty() && dt > 0.f) {
            
            // 1. Cho animation đầu tiên chạy
            sequence.front()->update(dt);

            // 2. Nếu nó chạy xong ngay lập tức (nhờ dt rất lớn)
            if (sequence.front()->isFinished()) {
                // Xóa nó đi, vòng lặp while sẽ tự động quay lại 
                // và lấy cái tiếp theo ra chạy luôn trong cùng 1 frame!
                sequence.pop();
            } 
            else {
                // 3. Nếu nó chưa chạy xong, nghĩa là nó cần thêm thời gian ở frame tiếp theo.
                // Ta phải break vòng lặp để nhường quyền render cho màn hình.
                break;
            }
        }
    }

    bool SequenceAnimation::isFinished() const {
        return sequence.empty();
    }

} // namespace UI::Animations