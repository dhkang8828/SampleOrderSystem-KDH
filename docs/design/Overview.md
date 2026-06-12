# Class Design Overview

## 전체 클래스 구조 및 의존 관계

```
┌─────────────────────────────────────────────────────────────┐
│                          View Layer                          │
│  MainView  SampleView  OrderView  ProductionView             │
│  MonitoringView  ReleaseView                                 │
└───────────────────────────┬─────────────────────────────────┘
                            │ uses
┌───────────────────────────▼─────────────────────────────────┐
│                       Controller Layer                       │
│  SampleController  OrderController                           │
│  ProductionController  ReleaseController                     │
└───────────────────────────┬─────────────────────────────────┘
                            │ uses
┌───────────────────────────▼─────────────────────────────────┐
│                      Repository Layer                        │
│  IRepository<T> (추상)                                       │
│  SampleRepository  OrderRepository  ProductionQueueRepository│
└───────────────────────────┬─────────────────────────────────┘
                            │ uses
┌───────────────────────────▼─────────────────────────────────┐
│                        Util Layer                            │
│  DBManager  IdGenerator  DateTimeUtil                        │
└─────────────────────────────────────────────────────────────┘
                            │ uses
┌───────────────────────────▼─────────────────────────────────┐
│                        Model Layer                           │
│  Sample  Order  ProductionJob                                │
│  OrderStatus (enum)  StockStatus (enum)                      │
└─────────────────────────────────────────────────────────────┘
```

## 클래스 목록

| 레이어 | 클래스 | 파일 |
|---|---|---|
| Model | Sample | Model_Sample.md |
| Model | Order | Model_Order.md |
| Model | ProductionJob | Model_ProductionJob.md |
| Model | OrderStatus (enum) | Model_Order.md |
| Model | StockStatus (enum) | Model_Sample.md |
| Repository | IRepository\<T\> | Repository_Base.md |
| Repository | SampleRepository | Repository_Sample.md |
| Repository | OrderRepository | Repository_Order.md |
| Repository | ProductionQueueRepository | Repository_ProductionQueue.md |
| Controller | SampleController | Controller_Sample.md |
| Controller | OrderController | Controller_Order.md |
| Controller | ProductionController | Controller_Production.md |
| Controller | ReleaseController | Controller_Release.md |
| View | MainView | View_Main.md |
| View | SampleView | View_Sample.md |
| View | OrderView | View_Order.md |
| View | ProductionView | View_Production.md |
| View | MonitoringView | View_Monitoring.md |
| View | ReleaseView | View_Release.md |
| Util | DBManager | Util_DBManager.md |
| Util | IdGenerator | Util_Common.md |
| Util | DateTimeUtil | Util_Common.md |

## 클린코드 / 확장성 원칙

### 1. IRepository\<T\> 추상화
- 모든 Repository는 `IRepository<T>`를 상속
- SQLite → 파일 / 다른 DB로 교체 시 Repository 구현체만 교체하면 됨
- Controller는 인터페이스에만 의존 (DIP 원칙)

### 2. IScheduler 인터페이스 (생산 스케줄링)
- 현재는 FIFO만 구현
- 향후 우선순위 큐(Priority), 라운드로빈(RoundRobin) 등으로 확장 가능

### 3. Sample 확장성
- Sample은 반도체 시료의 기본 속성만 보유
- 향후 시료 타입별(웨이퍼/에피택셜 등) 서브클래스 확장 가능하도록 설계

### 4. View / Controller 분리
- View는 입출력만 담당, 비즈니스 로직 없음
- Controller는 콘솔 출력 코드 없음
- 향후 GUI/Web으로 전환 시 View만 교체하면 됨
