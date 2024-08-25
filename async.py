import time
import asyncio


async def execCommand():
    print("Run command.")


async def main():
    future = execCommand()
    time.sleep(2)
    print("Skip command.")
    await future


asyncio.run(main())
