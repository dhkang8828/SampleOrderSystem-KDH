# Class Design: View Layer

## 포함 클래스
- MainView
- SampleView
- OrderView / ApprovalView
- MonitoringView
- ProductionView
- ReleaseView

---

## 설계 원칙
- View는 **입출력만** 담당, 비즈니스 로직 없음
- Controller를 통해서만 데이터 접근
- 향후 GUI/Web 전환 시 View만 교체하면 됨

---

## 1. MainView

```cpp
class MainView {
public:
    MainView(SampleController& sampleCtrl,
             OrderController& orderCtrl,
             ProductionController& productionCtrl);

    void show();
    void displayHeader();
    void displaySummary();
    void displayMenu();
    int getMenuChoice();

private:
    SampleController& sampleCtrl_;
    OrderController& orderCtrl_;
    ProductionController& productionCtrl_;
};
```

### 화면 구성
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

---

## 2. SampleView

```cpp
class SampleView {
public:
    explicit SampleView(SampleController& sampleCtrl);

    void show();
    void displayMenu();
    void showRegisterForm();
    void showSampleList();
    void showSearchForm();
    void displaySampleTable(const std::vector<Sample>& samples);

private:
    SampleController& sampleCtrl_;
};
```

### 화면 구성 - 시료 목록
```
  [1] 시료 관리
  [1] 시료 등록   [2] 시료 목록   [3] 시료 검색   [0] 뒤로

  등록 시료 목록  (총 12종)
  ID       시료명                평균 생산시간   수율    현재 재고
  S-001    실리콘 웨이퍼-8인치   0.5 min/ea     0.92    480 ea
  S-002    GaN 에피택셀-4인치    0.3 min/ea     0.78    220 ea
```

---

## 3. OrderView / ApprovalView

```cpp
class OrderView {
public:
    OrderView(OrderController& orderCtrl, SampleController& sampleCtrl);

    void show();
    void displayOrderResult(const Order& order);

private:
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
};

class ApprovalView {
public:
    explicit ApprovalView(OrderController& orderCtrl);

    void show();
    void displayReservedOrders();
    void displayApprovalResult(const Order& order);

private:
    OrderController& orderCtrl_;
};
```

### 화면 구성 - 주문 접수
```
  [2] 시료 주문
  시료 ID     > S-003
  고객명      > 삼성전자 파운드리
  주문 수량   > 200

  입력 내용 확인
  시료    SiC 파워기판-6인치 (S-003)
  고객    삼성전자 파운드리
  수량    200 ea

  [Y] 예약 접수   [N] 취소
  선택 > Y

  예약 접수 완료.
  주문번호   ORD-20260612-0001
  현재 상태  RESERVED
```

### 화면 구성 - 주문 승인/거절
```
  [3] 주문 승인/거절

  승인 대기 중인 예약 목록 (RESERVED)
  번호   주문번호    고객            시료                수량
  [1]    ORD-0041   LG이노텍        산화막 웨이퍼-SiO2  300 ea
  [2]    ORD-0042   SK하이닉스      실리콘 웨이퍼-8인치 150 ea

  승인할 번호 > 1

  재고 확인 중...
  시료      산화막 웨이퍼-SiO2   현재 재고  0 ea
  주문 수량  300 ea              부족분     300 ea

  재고 부족. 생산 라인에 등록합니다.
  상태 변경  RESERVED → PRODUCING
```

---

## 4. MonitoringView

```cpp
class MonitoringView {
public:
    MonitoringView(OrderController& orderCtrl, SampleController& sampleCtrl);

    void show();
    void displayMenu();
    void showOrderStatus();
    void showStockStatus();

private:
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
    std::string stockStatusToString(StockStatus status) const;
};
```

### 화면 구성
```
  [4] 모니터링   2026-06-12 09:32:15
  [1] 주문량 확인   [2] 재고량 확인   [0] 뒤로

  상태별 주문 현황
  RESERVED     3건
  CONFIRMED    8건
  PRODUCING    3건  ← 생산라인 대기
  RELEASED    18건

  재고 현황
  시료명                  재고       상태    잔여율
  실리콘 웨이퍼-8인치     480 ea     여유     80%
  GaN 에피택셀-4인치      220 ea     여유     44%
  SiC 파워기판-6인치       30 ea     부족      6%
  산화막 웨이퍼-SiO2        0 ea     고갈      0%
```

---

## 5. ProductionView

```cpp
class ProductionView {
public:
    ProductionView(ProductionController& productionCtrl,
                   OrderController& orderCtrl,
                   SampleController& sampleCtrl);

    void show();
    void displayCurrentJob();
    void displayWaitingQueue();
    void displayCompletePrompt();

private:
    ProductionController& productionCtrl_;
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
};
```

### 화면 구성
```
  [5] 생산라인 조회   FIFO 방식

  현재 처리 중
    주문번호   ORD-20260612-0038   시료  SiC 파워기판-6인치
    주문량     80 ea   재고  30 ea → 부족  50 ea → 실생산량  61 ea
    총 생산시간  48.8 min

  대기 중인 주문 (FIFO 순)
  순서   주문번호    시료                주문량    부족분   실생산량
  1      ORD-0040   산화막 웨이퍼-SiO2  150 ea    150 ea   190 ea
  2      ORD-0043   SiC 파워기판-6인치  200 ea    170 ea   206 ea

  * 실생산량 = ceil(부족분 / (수율 * 0.9))
```

---

## 6. ReleaseView

```cpp
class ReleaseView {
public:
    ReleaseView(ReleaseController& releaseCtrl, SampleController& sampleCtrl);

    void show();
    void displayConfirmedOrders();
    void displayReleaseResult(const Order& order);

private:
    ReleaseController& releaseCtrl_;
    SampleController& sampleCtrl_;
};
```

### 화면 구성
```
  [6] 출고 처리

  출고 가능 주문 (CONFIRMED)
  번호   주문번호             고객         시료                  수량
  [1]    ORD-20260612-0042   SK하이닉스   실리콘 웨이퍼-8인치   150 ea
  [2]    ORD-20260612-0035   DB하이텍     포토레지스트-PR7       400 ea

  출고할 번호 > 1

  출고 처리 완료.
  주문번호   ORD-20260612-0042
  출고수량   150 ea
  처리일시   2026-06-12 09:34:02
  상태       CONFIRMED → RELEASED
```
