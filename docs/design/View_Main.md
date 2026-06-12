# Class Design: MainView

## 개요

메인 메뉴 화면 출력 및 사용자 입력 처리 담당.

## 클래스 정의

```cpp
class MainView {
public:
    MainView(SampleController& sampleCtrl,
             OrderController& orderCtrl,
             ProductionController& productionCtrl);

    void show();                    // 메인 루프 진입점
    void displayHeader();           // ASCII 아트 + 시스템 타이틀
    void displaySummary();          // 현황 요약 (시료 수, 재고, 주문 수, 생산 대기)
    void displayMenu();             // 메뉴 항목 출력
    int getMenuChoice();            // 사용자 입력 받기

private:
    SampleController& sampleCtrl_;
    OrderController& orderCtrl_;
    ProductionController& productionCtrl_;
};
```

## 화면 구성

```
===============================================================
  반도체 시료 생산주문관리 시스템
===============================================================
  시스템 현황  2026-06-12 09:32:15

  등록 시료    12종       총 재고     2,840 ea
  전체 주문    36건       생산라인    3건 대기
---------------------------------------------------------------
  [1] 시료 관리              [2] 시료 주문
  [3] 주문 승인/거절         [4] 모니터링
  [5] 생산라인 조회          [6] 출고 처리
  [0] 종료
---------------------------------------------------------------
  선택 > _
```
