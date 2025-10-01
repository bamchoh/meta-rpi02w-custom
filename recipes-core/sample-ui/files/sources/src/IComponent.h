#pragma once

class IComponent {
public:
    // 純粋仮想関数
    virtual void Draw() = 0;

    // 仮想デストラクタは必須
    virtual ~IComponent() {}
};

