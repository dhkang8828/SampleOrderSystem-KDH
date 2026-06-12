# Class Design: ProductionView

## 개요

생산 라인 현황 화면 담당.

## 클래스 정의

```cpp
class ProductionView {
public:
    ProductionView(ProductionController& productionCtrl,
                   OrderController& orderCtrl,
                   SampleController& sampleCtrl);

    void show();                        // 생산라인 조회 화면
    void displayCurrentJob();           // 현재 생산 중인 작업 출력
    void displayWaitingQueue();         // 대기 중인 작업 목록 출력
    void displayCompletePrompt();       // 생산 완료 처리 프롬프트

private:
    ProductionController& productionCtrl_;
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
};
```

## 화면 구성

```
  [5] 생산라인 조회   FIFO 방식
  생산라인 1개 (단일 라인)   현재 상태: RUNNING

  현재 처리 중
    주문번호   ORD-20260612-0038   시료  SiC 파워기판-6인치
    주문량     80 ea   재고  30 ea → 부족  50 ea → 실생산량  61 ea
    총 생산시간  48.8 min

  대기 중인 주문 (FIFO 순)
  순서   주문번호       시료                  주문량    부족분   실생산량   예상 완료
  1      ORD-0040      산화막 웨이퍼-SiO2    150 ea    150 ea   190 ea    11:43
  2      ORD-0043      SiC 파워기판-6인치    200 ea    170 ea   206 ea    14:28

  * 부족분 = 주문량 - 재고,  실생산량 = ceil(부족분 / (수율 * 0.9))
```
