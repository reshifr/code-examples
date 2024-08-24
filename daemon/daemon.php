<?php

class DaemonException extends Exception {}

class Daemon
{
	public const EXIT_SUCCESS = 0;
	public const EXIT_FAILURE = 1;
	public const INVALID_TIMEOUT_ERROR_CODE = 11;
	public const CHILD_CREATING_ERROR_CODE = 12;
	public const DAEMON_CREATING_ERROR_CODE = 13;

	public function run(callable $callback, int $timeout)
	{
		if ($timeout <= 0) {
			throw new DaemonException(
				"Batas waktu tidak valid, tidak boleh negatif atau nol",
				Daemon::INVALID_TIMEOUT_ERROR_CODE
			);
		}

		$pid = pcntl_fork();
		if ($pid < 0) {
			/**
			 * Creating child process error.
			 */
			throw new DaemonException(
				"Terjadi kegagalan ketika membuat proses latar belakang",
				Daemon::CHILD_CREATING_ERROR_CODE
			);
		}
		if ($pid > 0) {
			/**
			 * Waiting child to create daemon.
			 */
			pcntl_waitpid($pid, $status);
			if (
				pcntl_wifsignaled($status) ||
				(pcntl_wifexited($status) && pcntl_wexitstatus($status) != Daemon::EXIT_SUCCESS)
			) {
				/**
				 * Creating daemon error.
				 */
				throw new DaemonException(
					"Proses latar belakang gagal dibuat",
					Daemon::DAEMON_CREATING_ERROR_CODE
				);
			}
			return;
		}

		if (posix_setsid() < 0) {
			exit(Daemon::EXIT_FAILURE);
		}

		fclose(STDIN);
		fclose(STDOUT);
		fclose(STDERR);

		$pid = pcntl_fork();
		if ($pid < 0) {
			exit(Daemon::EXIT_FAILURE);
		}
		if ($pid > 0) {
			exit(Daemon::EXIT_SUCCESS);
		}

		pcntl_async_signals(true);
		pcntl_signal(SIGALRM, function () {
			$this->logError("Proses dihentikan karena melebihi batas waktu.");
			exit(1);
		});
		pcntl_alarm($timeout);
		call_user_func($callback);
		pcntl_alarm(0);
		exit(Daemon::EXIT_SUCCESS);
	}

	public function logInfo($message)
	{
		file_put_contents('./info.log', date('Y-m-d H:i:s') . " - $message\n", FILE_APPEND);
	}

	public function logError($message)
	{
		file_put_contents('./error.log', date('Y-m-d H:i:s') . " - $message\n", FILE_APPEND);
	}
}

$daemon = new Daemon();
$daemon->run(function () use ($daemon) {
	for ($i = 0; $i < 100; ++$i) {
		$daemon->logInfo("Hello Daemon {$i}.");
		sleep(1);
	}
}, 10);
echo "Hello Parent." . PHP_EOL;
