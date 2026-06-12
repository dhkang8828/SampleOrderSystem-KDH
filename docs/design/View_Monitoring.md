# Class Design: MonitoringView

## 개요

주문 현황 및 재고 상태 모니터링 화면 담당.

## 클래스 정의

```cpp
class MonitoringView {
public:
    MonitoringView(OrderController& orderCtrl, SampleController& sampleCtrl);

    void show();                    // 모니터링 서브메뉴 루프
    void displayMenu();
    void showOrderStatus();         // 상태별 주문 수 출력
    void showStockStatus();         // 시료별 재고 + 상태 출력

private:
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
    std::string stockStatusToString(StockStatus status) const;
};
```

## 화면 구성

```
  [4] 모니터링   2026-06-12 09:32:15
  [1] 주문량 확인   [2] 재고량 확인   [0] 뒤로

  상태별 주문 현황
  RESERVED    3건
  CONFIRMED   8건
  PRODUCING   3건  ← 생산라인 대기
  RELEASED   18건

  재고 현황
  시료명                  재고       상태    잔여율
  실리콘 웨이퍼-8인치     480 ea     여유    80%
  GaN 에피택셀-4인치      220 ea     여유    44%
  SiC 파워기판-6인치       30 ea     부족     6%
  산화막 웨이퍼-SiO2        0 ea     고갈     0%
```
