# Class Design: IRepository<T>

## 개요

모든 Repository가 상속하는 추상 인터페이스.  
데이터 저장소(SQLite)를 교체하거나 테스트용 Mock Repository를 주입할 수 있도록 DIP(의존 역전 원칙)를 적용한다.

## 확장성 설계

```
IRepository<T>  (추상 인터페이스)
  ├── SampleRepository      (SQLite 구현체)
  ├── OrderRepository       (SQLite 구현체)
  └── ProductionQueueRepository (SQLite 구현체)

향후 교체 시:
  ├── SampleFileRepository  (파일 기반 구현체)
  └── SampleMockRepository  (테스트용 Mock)
```

- Controller는 `IRepository<T>*`에만 의존 → 구현체 교체 시 Controller 수정 불필요

## 클래스 정의

```cpp
template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool save(const T& entity) = 0;
    virtual std::optional<T> findById(const std::string& id) = 0;
    virtual std::vector<T> findAll() = 0;
    virtual bool update(const T& entity) = 0;
    virtual bool remove(const std::string& id) = 0;
};
```

## 각 구현체의 책임

| 메서드 | 설명 |
|---|---|
| `save` | 새 엔티티 저장 (INSERT) |
| `findById` | ID로 단건 조회, 없으면 std::nullopt |
| `findAll` | 전체 목록 조회 |
| `update` | 기존 엔티티 수정 (UPDATE) |
| `remove` | ID로 삭제 (DELETE) |

## DBManager 의존

- 모든 SQLite Repository는 `DBManager` 싱글톤을 통해 DB 커넥션을 획득
- DB 초기화(테이블 생성)는 `DBManager::initialize()`에서 일괄 처리
