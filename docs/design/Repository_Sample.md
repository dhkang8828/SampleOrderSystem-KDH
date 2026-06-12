# Class Design: SampleRepository

## 개요

Sample 도메인의 SQLite 영속성 처리 담당.  
`IRepository<Sample>`을 상속하여 CRUD를 구현한다.

## 클래스 정의

```cpp
class SampleRepository : public IRepository<Sample> {
public:
    explicit SampleRepository(DBManager& dbManager);

    // IRepository<Sample> 구현
    bool save(const Sample& sample) override;
    std::optional<Sample> findById(const std::string& sampleId) override;
    std::vector<Sample> findAll() override;
    bool update(const Sample& sample) override;
    bool remove(const std::string& sampleId) override;

    // 추가 조회 메서드
    std::vector<Sample> findByName(const std::string& keyword);  // 이름 부분 검색
    bool updateStock(const std::string& sampleId, int newStock); // 재고만 업데이트
    bool existsById(const std::string& sampleId);               // ID 존재 여부

private:
    DBManager& dbManager_;
    Sample rowToSample(sqlite3_stmt* stmt) const;  // DB row → Sample 변환
};
```

## SQLite 테이블 스키마

```sql
CREATE TABLE IF NOT EXISTS samples (
    sample_id            TEXT PRIMARY KEY,
    name                 TEXT NOT NULL,
    avg_production_time  REAL NOT NULL,
    yield_rate           REAL NOT NULL,
    stock                INTEGER NOT NULL DEFAULT 0
);
```

## 주요 메서드 동작

| 메서드 | SQL | 설명 |
|---|---|---|
| `save` | INSERT INTO samples | 중복 ID 시 false 반환 |
| `findById` | SELECT WHERE sample_id = ? | 없으면 nullopt |
| `findAll` | SELECT * FROM samples | 전체 시료 목록 |
| `update` | UPDATE samples SET ... | 전체 필드 업데이트 |
| `remove` | DELETE WHERE sample_id = ? | 주문이 있는 시료 삭제 주의 |
| `findByName` | SELECT WHERE name LIKE ? | 부분 일치 검색 |
| `updateStock` | UPDATE SET stock = ? | 재고만 변경 |
| `existsById` | SELECT COUNT(*) WHERE ... | 존재 여부만 확인 |
