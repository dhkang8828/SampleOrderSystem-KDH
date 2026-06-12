# Class Design: OrderView / ApprovalView

## OrderView - 시료 주문 화면

```cpp
class OrderView {
public:
    explicit OrderView(OrderController& orderCtrl, SampleController& sampleCtrl);

    void show();                    // 주문 접수 폼
    void displayOrderResult(const Order& order);  // 접수 완료 결과 출력

private:
    OrderController& orderCtrl_;
    SampleController& sampleCtrl_;
};
```

## ApprovalView - 주문 승인/거절 화면

```cpp
class ApprovalView {
public:
    explicit ApprovalView(OrderController& orderCtrl);

    void show();                    // 승인/거절 서브메뉴 루프
    void displayReservedOrders();   // RESERVED 목록 출력
    void displayApprovalResult(const Order& order);  // 처리 결과 출력

private:
    OrderController& orderCtrl_;
};
```

## 화면 구성 - 주문 접수

```
---------------------------------------------------------------
  [2] 시료 주문

  시료 ID     > S-003
  고객명      > 삼성전자 파운드리
  주문 수량   > 200

  입력 내용 확인
  시료         SiC 파워기판-6인치 (S-003)
  고객         삼성전자 파운드리
  수량         200 ea

  [Y] 예약 접수   [N] 취소
  선택 > Y

  예약 접수 완료.
  주문번호   ORD-20260612-0001
  현재 상태  RESERVED
```

## 화면 구성 - 주문 승인/거절

```
  승인 대기 중인 예약 목록 (RESERVED)
  번호   주문번호       고객              시료                수량      상태
  [1]    ORD-0041      LG이노텍          산화막 웨이퍼-SiO2  300 ea    RESERVED
  [2]    ORD-0042      SK하이닉스        실리콘 웨이퍼-8인치 150 ea    RESERVED

  승인할 번호 > 1

  재고 확인 중...
  시료     산화막 웨이퍼-SiO2   현재 재고  0 ea
  주문 수량 300 ea              부족분     300 ea

  재고 부족. 생산 라인에 등록합니다.
  상태 변경  RESERVED → PRODUCING
```
