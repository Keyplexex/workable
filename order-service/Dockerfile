FROM eclipse-temurin:17-jdk AS build
WORKDIR /workspace
COPY gradlew settings.gradle build.gradle ./
COPY gradle ./gradle
COPY carshow-events ./carshow-events
COPY order-service ./order-service
RUN chmod +x ./gradlew && ./gradlew :order-service:bootJar --no-daemon -x test

FROM eclipse-temurin:17-jre
WORKDIR /app
COPY --from=build /workspace/order-service/build/libs/*.jar /app/app.jar
EXPOSE 8081
ENTRYPOINT ["java", "-jar", "/app/app.jar"]
