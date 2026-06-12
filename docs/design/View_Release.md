# Class Design: ReleaseView

## 개요

출고 처리 화면 담당.

## 클래스 정의

```cpp
class ReleaseView {
public:
    ReleaseView(ReleaseController& releaseCtrl, SampleController& sampleCtrl);

    void show();                        // 출고 처리 화면
    void displayConfirmedOrders();      // 출고 가능 주문 목록 출력
    void displayReleaseResult(const Order& order); // 처리 결과 출력

private:
    ReleaseController& releaseCtrl_;
    SampleController& sampleCtrl_;
};
```

## 화면 구성

```
  [6] 출고 처리

  출고 가능 주문 (CONFIRMED)
  번호   주문번호          고객         시료                  수량
  [1]    ORD-20260612-0042  SK하이닉스  실리콘 웨이퍼-8인치   150 ea
  [2]    ORD-20260612-0035  DB하이텍    포토레지스트-PR7       400 ea

  출고할 번호 > 1

  출고 처리 완료.
  주문번호   ORD-20260612-0042
  출고수량   150 ea
  처리일시   2026-06-12 09:34:02
  상태       CONFIRMED → RELEASED
```
