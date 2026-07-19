# AssaultCube Mod Menu

오픈소스 FPS AssaultCube의 메모리 구조를 분석하고, Windows API로 게임 메모리를 조작하며 리버스 엔지니어링을 학습하기 위해 만든 외부 트레이너이다.

> **교육용 목적**
>
> 이 프로젝트는 리버스 엔지니어링과 Windows 메모리 API 학습을 위한 것으로, 싱글플레이/오프라인 환경에서만 사용해야 한다.
>
> 온라인 멀티플레이에서 사용하면 다른 플레이어에게 피해를 주고, 게임 이용약관에 위반될 수 있다.

## 코드 설명

1. **`FindPidByName`** — `CreateToolhelp32Snapshot`으로 실행 중인 프로세스 목록을 반복하여 `ac_client.exe`의 PID를 찾는다.
2. **`GetModuleBase`** — 찾은 프로세스의 모듈 목록에서 `ac_client.exe`의 베이스 주소를 구한다.
3. **`setHealth`** — 베이스 주소에 오프셋(`0x0017E254`)을 더한 위치에서 포인터를 읽고, 거기에 `0xEC`(offset)를 더한 최종 주소에 원하는 체력 값을 쓴다.
```
baseAddr + 0x0017E254  →  [player pointer]  →  + 0xEC  →  health
```
4. **`setAmmo`** - 베이스 주소에 오프셋(`0x0018AC00`)을 더한 위치에서 포인터를 읽고, 거기에 `0x140`을 더한 최종 주소에 원하는 탄약 값을 쓴다.
```
baseAddr + 0x0018AC00  →  [player pointer]  →  + 0x140  →  health
```
