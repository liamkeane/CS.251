;; The function sum takes a list of integers as an argument and returns the sum of each element in the list.
(define sum
  (lambda (lst)
    (if (null? lst)
	0
	(+ (car lst) (sum (cdr lst))))))

;; The funtion keep-first-n takes an integer (n) and a list as its arguments and returns a list of the first
;; n elements in the given list. If n is less than or equal to 0 or it is greator than the length of the list,
;; it will return an empty list.
(define keep-first-n
  (lambda (n lst)
    (cond [(null? lst) '()]
	  [(<= n 0) '()]
	  

(define keep-first-n-helper
  (lambda (n lst)
    (cond ((<= n 0) #f)
	(else (cons (car lst) (keep-first-n-helper (- n 1) (cdr lst)))))))
