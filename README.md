## 개요
이 프로젝트는 C 언어를 사용하여 RISC-V 아키텍처를 기반으로 하는 두 가지 주요 작업을 수행하는 것을 목표로 합니다.

## 내용
### PROJECT1: Interpret RISC-V binary code
- PROJECT1 에서는 C 언어를 사용하여 RISC-V 명령어 집합을 해석하는 프로그램을 개발합니다. 
- RISC-V 머신 코드가 포함된 바이너리 파일을 입력으로 받아, 해당 명령어들을 디코딩하여 어셈블리 언어로 표현된 결과를 출력합니다. 

### PROJECT2: Simulate a single-cycle CPU
- 두 번째 프로젝트에서는 C 언어를 사용하여 RISC-V 명령어 집합의 일부를 지원하는 명령어 시뮬레이터를 구현합니다.
- 이 시뮬레이션에서는 CPU의 각 구성 요소(예: ALU, 레지스터 파일, 메모리 등)를 구현하고, 명령어 실행의 전체 과정을 시뮬레이션합니다. 
- 이 프로그램은 바이너리 파일에서 RISC-V 명령어를 읽어 들여, 각 명령어를 하나씩 순차적으로 실행합니다. 
- 실행이 완료되면, 프로그램은 현재 레지스터들의 값을 출력하며, 실행 결과와 주어진 정답 파일을 비교해보며 성능을 확인할 수 있습니다.
