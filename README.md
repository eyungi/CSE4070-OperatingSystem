# 운영체제 (CSE4070) 핀토스 프로젝트 구현 요약

이 리포지토리에는 Pintos 기반 ‘운영체제’ 과목의 5개 프로젝트(Proj0\~Proj4)가 포함되어 있다. 각 프로젝트의 핵심 요구사항과 실행 방법을 요약하였다.

```
CSE4070-OperatingSystem/
├── prj_0/      # Project 0: 알람 동작 캡처 이미지 제출
├── prj_1/      # Project 1: 사용자 프로그램 실행 지원
├── prj_2/      # Project 2: 파일 시스템 시스템콜 구현
├── prj_3/      # Project 3: 스레드 스케줄러 및 동기화
└── prj_4/      # Project 4: 가상 메모리 및 페이징
```

---

## Project 0: 알람 작동 캡처 (Submission Only)

- **요구사항**: Pintos에서 `alarm-multiple` 테스트 실행 결과 캡처<br>

  - 화면에 학번과 "Powering off..." 메시지 포함되어야 함

---

## Project 1: User Program (1)

- **주제**: 인자 전달, 시스템콜 핸들러, 프로세스 종료 메시지 구현
- **핵심 요구사항**:

  1. **인자 전달**: `setup_stack`에서 80x86 Calling Convention 기반 argv 구성
  2. **시스템콜**: `halt`, `exit`, `exec`, `wait`, `read`, `write` 구현
  3. **프로세스 종료 메시지**: 종료 시 `Process <name>: exit(<status>)` 출력

- **실행**:

  ```bash
  cd proj1_userprog1/src/userprog
  make check                    # 자동 테스트
  pintos --filesys-size=2 -p ../examples/echo -a echo -- -q run echo x
  ```

---

## Project 2: User Program (2)

- **주제**: 파일 시스템 관련 시스템콜 구현 및 동기화
- **핵심 요구사항**:

  - `create`, `remove`, `open`, `close`, `filesize`, `read`, `write`, `seek`, `tell` 시스템콜
  - `read(0)`은 `input_getc()`, `write(1)`은 `putbuf()` 사용
  - 파일 핸들링 시 임계영역 보호(락/세마포어)

- **실행**:

  ```bash
  cd proj2_userprog2/src/userprog
  make check
  ```

---

## Project 3: Threads

- **주제**: 스레드 스케줄링 정책 및 동기화 구현
- **핵심 요구사항**:

  1. **Alarm Clock**: `timer_sleep` 최적화 (스레드 블록/웨이크업 큐)
  2. **Priority Scheduling**: 우선순위 기반 선점 스케줄러, Aging
  3. **Advanced Scheduler (BSD)**: MLFQ 또는 NICE 기반 스케줄링 (추가 점수)

- **실행**:

  ```bash
  cd proj3_threads/src/threads
  make check
  pintos -v -- -q run alarm-multiple
  ```

---

## Project 4: Virtual Memory

- **주제**: Demand Paging, Swap, Supplemental Page Table 구현
- **핵심 요구사항**:

  1. **Supplemental Page Table**: PTE 확장, 해시/리스트 기반 관리
  2. **Page Fault Handler**: Lazy loading, Stack Growth, `handle_mm_fault()` 구현
  3. **Swap**: Second-Chance 알고리즘, Swap Table 및 Disk 블록 I/O

- **실행**:

  ```bash
  cd proj4_vm/src/vm
  make check
  pintos -v -- -q run pt-grow-stack
  ```

---
